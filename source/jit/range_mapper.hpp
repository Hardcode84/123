#ifndef RANGE_MAPPER_HPP
#define RANGE_MAPPER_HPP

#include "util/types.hpp"

namespace jit
{

struct MapperParams
{
    util::MSpans spans;
    gsl::not_null<void*> data;
};

struct IdentityRangeMapper
{
    static util::MDimensions mapTile(const MapperParams& params, const util::MDimensions& src);
    static gsl::not_null<void*> mapTilePtr(const MapperParams& params, gsl::not_null<void*> srcPtr);
};

struct OffsetMapper
{
    util::MOffset offset;

    util::MDimensions mapTile(const MapperParams& params, const util::MDimensions& src) const;
    gsl::not_null<void*> mapTilePtr(const MapperParams& params, gsl::not_null<void*> srcPtr) const;
};

}

#endif // RANGE_MAPPER_HPP
