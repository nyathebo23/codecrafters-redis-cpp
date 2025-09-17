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

struct NormalizedCoords {
    long longitude;
    long latitude;
};

struct SpatialCoords{
    double longitude;
    double latitude;
    std::optional<std::string> error;
};


class GeospatialCommandsProcessing {
    private:
        static std::string convert_geocode_into_resp_string(const long& geocode);
        static long compact_int64_to_int32(const long& value); 
        static long interleave(const long& x, const long& y);
        static long spread_int32_to_int64(const long& value);
        static double haversine(const SpatialCoords& coords1, const SpatialCoords& coords2);
        static SpatialCoords get_coords_from_compact_long(const long& value);
        static SpatialCoords validate_coordinates(const std::string& longitude, const std::string& latitude);
        static NormalizedCoords normalize_coordinates(const SpatialCoords& coords);

    public:
        static std::string geoadd(const std::vector<std::string>& extras);
        static std::string geopos(const std::vector<std::string>& extras);
        static std::string geodist(const std::vector<std::string>& extras);
        static std::string geosearch(const std::vector<std::string>& extras);
};

#endif