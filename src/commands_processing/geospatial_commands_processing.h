#ifndef GEOSPATIAL_COMMANDS_PROCESSING_H
#define GEOSPATIAL_COMMANDS_PROCESSING_H

#include <string>
#include <optional>
#include <vector>

const double MIN_LATITUDE = -85.05112878;
const double MAX_LATITUDE = 85.05112878;
const double MIN_LONGITUDE = -180;
const double MAX_LONGITUDE = 180;

const double LATITUDE_RANGE = MAX_LATITUDE - MIN_LATITUDE;
const double LONGITUDE_RANGE = MAX_LONGITUDE - MIN_LONGITUDE;

struct SpatialCoord {
    long longitude;
    long latitude;
    std::optional<std::string> error;
};


class GeospatialCommandsProcessing {
    private:
        static std::string convert_grid_numbers_to_coordinates(long grid_lat_num, long grid_lon_num);
        static long compact_int64_to_int32(long value); 
        static long interleave(long x, long y);
        static long spread_int32_to_int64(long value);
        static double haversine(double lat1, double lon1, double lat2, double lon2);
        static SpatialCoord validate_and_normalize_coordinates(std::string longitude, std::string latitude);

    public:
        static std::string geoadd(std::vector<std::string> extras);
        static std::string geopos(std::vector<std::string> extras);
        static std::string geodist(std::vector<std::string> extras);
        static std::string geosearch(std::vector<std::string> extras);
};

#endif