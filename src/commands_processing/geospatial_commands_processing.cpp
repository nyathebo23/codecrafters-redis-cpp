#include "geospatial_commands_processing.h"
#include "sortedsets_commands_processing.h"
#include "command_processing.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "../utils/encode/simple_data_parser_enc.h"
#include "../utils/encode/small_aggregate_parser_enc.h"
#include "../utils/encode/array_parser_enc.h"
#include "../globals_datas/global_datas.h"
#include "../utils/resp_constants.h"
#include <cmath> 


NormalizedCoords GeospatialCommandsProcessing::normalize_coordinates(SpatialCoords coords) {
    double normalizedLatitude = std::pow(2.0, 26) * (coords.latitude - MIN_LATITUDE) / LATITUDE_RANGE;
    double normalizedLongitude = std::pow(2.0, 26) * (coords.longitude - MIN_LONGITUDE) / LONGITUDE_RANGE;
    long normLatitude = static_cast<long>(normalizedLatitude);
    long normLongitude = static_cast<long>(normalizedLongitude);
    NormalizedCoords normCoords = {
        .longitude = normLongitude,
        .latitude = normLatitude,
    };
    return normCoords;
}

SpatialCoords GeospatialCommandsProcessing::validate_coordinates(std::string longitude, std::string latitude) {
    SpatialCoords coordErrorDefault = {
        .longitude = 0,
        .latitude = 0,
        .error = "ERR invalid longitude,latitude pair " + longitude + ", " + latitude
    };
    
    try {
        double lon = std::stod(longitude);
        double lat = std::stod(latitude);
        if (lon > MAX_LONGITUDE || lon < MIN_LONGITUDE) return coordErrorDefault;
        if (lat > MAX_LATITUDE || lat < MIN_LATITUDE) return coordErrorDefault;
        SpatialCoords normCoords = {
            .longitude = lon,
            .latitude = lat,
            .error = std::nullopt
        };
        return normCoords;
    }
     catch (const std::invalid_argument& e) {
        return coordErrorDefault;
    } catch (const std::out_of_range& e) {
        return coordErrorDefault;
    }
}



std::string GeospatialCommandsProcessing::geoadd(std::vector<std::string> extras) {
    if (extras.size() != 4) {
        return CommandProcessing::params_count_error("geoadd");
    }
    SpatialCoords coords = validate_coordinates(extras[1], extras[2]);
    if (coords.error.has_value()) return parse_encode_error_msg(coords.error.value());
    NormalizedCoords normCoords = normalize_coordinates(coords);
    std::string score = std::to_string(interleave(normCoords.latitude, normCoords.longitude));
    std::vector<std::string> zparams = {extras[0], score, extras[3]};
    return SortedSetsCommandsProcessing::zadd(zparams);
}

std::string GeospatialCommandsProcessing::geopos(std::vector<std::string> extras) {
    if (extras.size() < 2) {
        return CommandProcessing::params_count_error("geopos");
    }
    std::string locationKey = extras[0];
    int nbLocations = extras.size() - 1;
    std::string resp = "*" + std::to_string(nbLocations) + "\r\n";
    for (int i = 1; i <= nbLocations; i++) {
        std::string zscore = GlobalDatas::sortedSets.zscore(locationKey, extras[i]);
        if (zscore == "") resp += NULL_ARRAY;
        else {
            long geocode = std::stol(zscore);
            resp += convert_geocode_into_resp_string(geocode);
        };
    }
    return resp;
}

std::string GeospatialCommandsProcessing::geodist(std::vector<std::string> extras) {
    if (extras.size() != 3) {
        return CommandProcessing::params_count_error("geodist");
    }
    std::string locationKey = extras[0];
    std::string location1 = extras[1];
    std::string location2 = extras[2];
    std::string zgeocode1 = GlobalDatas::sortedSets.zscore(locationKey, location1);
    std::string zgeocode2 = GlobalDatas::sortedSets.zscore(locationKey, location2);
    std::string respErr = zgeocode1 == "" ? location1 : "";
    std::string err2 = zgeocode2 == "" ? location2 : "";
    respErr += (respErr == "" ? err2 : ", " + err2);
    if (respErr != "") {
        respErr += " not found in location key " + locationKey;
        return parse_encode_error_msg(respErr);
    }
    SpatialCoords coords1 = get_coords_from_compact_long(std::stol(zgeocode1));
    SpatialCoords coords2 = get_coords_from_compact_long(std::stol(zgeocode2));
    double dist = haversine(coords1, coords2);
    return parse_encode_bulk_string(std::to_string(dist));
}

std::string GeospatialCommandsProcessing::geosearch(std::vector<std::string> extras) {
    if (extras.size() < 7) {
        return CommandProcessing::params_count_error("geosearch");
    }
    std::string locationKey = extras[0];
    SpatialCoords coords = validate_coordinates(extras[2], extras[3]);
    if (coords.error.has_value()) return parse_encode_error_msg(coords.error.value());
    double radius = std::stod(extras[5]);
    std::string unit = extras[6];
    if (unit == "mi") {
        radius *= 1609.34;
    }
    else if (unit == "km") {
        radius *= 1000;
    }
    std::vector<std::string> result;
    std::list<SortedSetElement> locationNames = GlobalDatas::sortedSets.zall(locationKey);
    for (SortedSetElement item: locationNames) {
        long geocode = static_cast<long>(item.score);
        SpatialCoords locationCoords = get_coords_from_compact_long(geocode);
        double dist = haversine(coords, locationCoords);
        if (dist < radius) {
            result.push_back(item.member);
        }
    }
    return parse_encode_string_array(result);
}

double GeospatialCommandsProcessing::haversine(SpatialCoords coords1, SpatialCoords coords2) {
   double radius = 6372797.560856;
   double PI_180 = M_PI / 180;
   double lat1 = coords1.latitude * PI_180;
   double lat2 = coords2.latitude * PI_180;
   double dLat =  lat2 - lat1;
   double dLon = (coords2.longitude - coords1.longitude) * PI_180;
   double a = std::pow(std::sin(dLat / 2), 2) + std::cos(lat1) * std::cos(lat2) * std::pow(std::sin(dLon / 2), 2);
   double c = 2 * std::asin(std::sqrt(a));
   return radius * c;
};

long GeospatialCommandsProcessing::interleave(long x, long y) {
    //First, the values are spread from 32-bit to 64-bit integers.
    //This is done by inserting 32 zero bits in-between.
    
    //Before spread: x1  x2  ...  x31  x32
    //After spread:  0   x1  ...   0   x16  ... 0  x31  0  x32
    long xLong = spread_int32_to_int64(x);
    long yLong = spread_int32_to_int64(y);

    //The y value is then shifted 1 bit to the left.
    //Before shift: 0   y1   0   y2 ... 0   y31   0   y32
    //After shift:  y1   0   y2 ... 0   y31   0   y32   0
    long yShifted = yLong << 1;

    //Next, x and y_shifted are combined using a bitwise OR.
    
    //Before bitwise OR (x): 0   x1   0   x2   ...  0   x31    0   x32
    //Before bitwise OR (y): y1  0    y2  0    ...  y31  0    y32   0
    //After bitwise OR     : y1  x2   y2  x2   ...  y31  x31  y32  x32
    return xLong | yShifted;
}


//Spreads a 32-bit integer to a 64-bit integer by inserting
//32 zero bits in-between.

//Before spread: x1  x2  ...  x31  x32
//After spread:  0   x1  ...   0   x16  ... 0  x31  0  x32
long GeospatialCommandsProcessing::spread_int32_to_int64(long value) {
    //Ensure only lower 32 bits are non-zero.
    long val = value & 0xFFFFFFFF;

    //Bitwise operations to spread 32 bits into 64 bits with zeros in-between
    val = (val | (val << 16)) & 0x0000FFFF0000FFFF;
    val = (val | (val << 8))  & 0x00FF00FF00FF00FF;
    val = (val | (val << 4))  & 0x0F0F0F0F0F0F0F0F;
    val = (val | (val << 2))  & 0x3333333333333333;
    val = (val | (val << 1))  & 0x5555555555555555;

    return val;
}

long GeospatialCommandsProcessing::compact_int64_to_int32(long v) {
    //Keep only the bits in even positions
    long val = v & 0x5555555555555555;

    //Before masking: w1   v1  ...   w2   v16  ... w31  v31  w32  v32
    //After masking: 0   v1  ...   0   v16  ... 0  v31  0  v32

    //Where w1, w2,..w31 are the digits from longitude if we're compacting latitude, or digits from latitude if we're compacting longitude
    //So, we mask them out and only keep the relevant bits that we wish to compact

    //------
    //Reverse the spreading process by shifting and masking
    val = (val | (val >> 1)) & 0x3333333333333333;
    val = (val | (val >> 2)) & 0x0F0F0F0F0F0F0F0F;
    val = (val | (val >> 4)) & 0x00FF00FF00FF00FF;
    val = (val | (val >> 8)) & 0x0000FFFF0000FFFF;
    val = (val | (val >> 16)) & 0x00000000FFFFFFFF;

    return val;
    //Before compacting: 0   v1  ...   0   v16  ... 0  v31  0  v32
    //After compacting: v1  v2  ...  v31  v32
    //-----
}



std::string GeospatialCommandsProcessing::convert_geocode_into_resp_string(long geocode) {
    SpatialCoords coords = get_coords_from_compact_long(geocode);
    std::ostringstream latOstream;
    std::ostringstream lonOstream;
    latOstream << std::setprecision(17) << coords.latitude;
    lonOstream << std::setprecision(17) << coords.longitude;
    std::string latitudeEnc = parse_encode_bulk_string(latOstream.str());
    std::string longitudeEnc = parse_encode_bulk_string(lonOstream.str());

    std::string resp = "*2\r\n" + longitudeEnc + latitudeEnc;     
    return resp;
}

SpatialCoords GeospatialCommandsProcessing::get_coords_from_compact_long(long value) {
    long transformedLon = value >> 1;
    long transformedLat = value;
    long gridLonNum = compact_int64_to_int32(transformedLon);
    long gridLatNum = compact_int64_to_int32(transformedLat);
    // Calculate the grid boundaries
    double gridLatitudeMin = MIN_LATITUDE + LATITUDE_RANGE * (gridLatNum / std::pow(2.0, 26));
    double gridLatitudeMax = MIN_LATITUDE + LATITUDE_RANGE * ((gridLatNum + 1) / std::pow(2.0, 26));
    double gridLongitudeMin = MIN_LONGITUDE + LONGITUDE_RANGE * (gridLonNum / std::pow(2.0, 26));
    double gridLongitudeMax = MIN_LONGITUDE + LONGITUDE_RANGE * ((gridLonNum + 1) / std::pow(2.0, 26));
    
    // Calculate the center point of the grid cell
    double latitude = (gridLatitudeMin + gridLatitudeMax) / 2;
    double longitude = (gridLongitudeMin + gridLongitudeMax) / 2;
    SpatialCoords coords = {
        .longitude = longitude,
        .latitude = latitude
    };
    return coords;
    //The decoded coordinates represent the center of a grid cell, not the exact original coordinates. 
    //This is because the encoding process truncates coordinates to grid cells. The precision depends on the grid resolution 
    //(which is determined by the 26-bit normalization).
}