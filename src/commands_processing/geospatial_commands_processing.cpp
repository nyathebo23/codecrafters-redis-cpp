#include "geospatial_commands_processing.h"
#include "sortedsets_commands_processing.h"
#include "command_processing.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "../utils/encode/simple_data_parser_enc.h"
#include "../utils/encode/small_aggregate_parser_enc.h"
#include "../globals_datas/global_datas.h"
#include "../utils/resp_constants.h"

SpatialCoord GeospatialCommandsProcessing::validate_and_normalize_coordinates(std::string longitude, std::string latitude) {
    SpatialCoord coordErrorDefault = {
        .longitude = LONG_MAX,
        .latitude = LONG_MAX,
        .error = "ERR invalid longitude,latitude pair " + longitude + ", " + latitude
    };
    
    try {
        double lon = std::stod(longitude);
        double lat = std::stod(latitude);
        if (lon > MAX_LONGITUDE || lon < MIN_LONGITUDE) return coordErrorDefault;
        if (lat > MAX_LATITUDE || lat < MIN_LATITUDE) return coordErrorDefault;
        double normalizedLatitude = (2^26) * (lat - MIN_LATITUDE) / LATITUDE_RANGE;
        double normalizedLongitude = (2^26) * (lon - MIN_LONGITUDE) / LONGITUDE_RANGE;
        long normLatitude = static_cast<long>(normalizedLatitude);
        long normLongitude = static_cast<long>(normalizedLongitude);
        SpatialCoord normCoords = {
            .longitude = normLongitude,
            .latitude = normLatitude,
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
    SpatialCoord coords = validate_and_normalize_coordinates(extras[1], extras[2]);
    if (coords.error.has_value()) return parse_encode_error_msg(coords.error.value());
    std::string score = std::to_string(interleave(coords.latitude, coords.longitude));
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
        if (zscore == "") resp += NULL_BULK_STRING;
        else {
            long geocode = std::stol(zscore);
            long transformedLon = geocode >> 1;
            long transformedLat = geocode;
            transformedLon = compact_int64_to_int32(transformedLon);
            transformedLat = compact_int64_to_int32(transformedLat);
            resp += convert_grid_numbers_to_coordinates(transformedLat, transformedLon);
        };
    }
    return resp;
}

std::string GeospatialCommandsProcessing::geodist(std::vector<std::string> extras) {
    if (extras.size() != 3) {
        return CommandProcessing::params_count_error("geodist");
    }

    return "";
}

std::string GeospatialCommandsProcessing::geosearch(std::vector<std::string> extras) {
    if (extras.size() < 7) {
        return CommandProcessing::params_count_error("geosearch");
    }

    return "";
}

double GeospatialCommandsProcessing::haversine(double lat1, double lon1, double lat2, double lon2) {

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

std::string GeospatialCommandsProcessing::convert_grid_numbers_to_coordinates(long gridLatNum, long gridLonNum) {
    // Calculate the grid boundaries
    double gridLatitudeMin = MIN_LATITUDE + LATITUDE_RANGE * (gridLatNum / (2^26));
    double gridLatitudeMax = MIN_LATITUDE + LATITUDE_RANGE * ((gridLatNum + 1) / (2^26));
    double gridLongitudeMin = MIN_LONGITUDE + LONGITUDE_RANGE * (gridLonNum / (2^26));
    double gridLongitudeMax = MIN_LONGITUDE + LONGITUDE_RANGE * ((gridLonNum + 1) / (2^26));
    
    // Calculate the center point of the grid cell
    double latitude = (gridLatitudeMin + gridLatitudeMax) / 2;
    double longitude = (gridLongitudeMin + gridLongitudeMax) / 2;
    std::ostringstream latOstream;
    std::ostringstream lonOstream;
    latOstream << std::setprecision(17) << latitude;
    lonOstream << std::setprecision(17) << longitude;
    std::string latitudeEnc = parse_encode_bulk_string(latOstream.str());
    std::string longitudeEnc = parse_encode_bulk_string(lonOstream.str());

    std::string resp = "*2\r\n" + longitudeEnc + latitudeEnc;     
    return resp;

    //The decoded coordinates represent the center of a grid cell, not the exact original coordinates. 
    //This is because the encoding process truncates coordinates to grid cells. The precision depends on the grid resolution 
    //(which is determined by the 26-bit normalization).
}