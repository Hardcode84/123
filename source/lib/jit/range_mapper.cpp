#include "range_mapper.hpp"

namespace jit
{

util::MDimensions IdentityRangeMapper::mapTile(const MapperParams& params, const util::MDimensions& src)
{
    (void)params;
    return src;
}

gsl::not_null<void*> IdentityRangeMapper::mapTilePtr(const MapperParams& params, gsl::not_null<void*> srcPtr)
{
    (void)params;
    return srcPtr;
}

util::MDimensions OffsetMapper::mapTile(const MapperParams& params, const util::MDimensions& src) const
{
    assert(offset.size() == params.spans.size());
    assert(offset.size() == src.size());
    util::MDimensions ret;
    for(size_t i = 0; i < offset.size(); ++i)
    {
        ret[i].begin = src[i].begin + offset[i];
        ret[i].size = src[i].size;
    }
    return ret;
}

gsl::not_null<void*> OffsetMapper::mapTilePtr(const MapperParams& params, gsl::not_null<void*> srcPtr) const
{
    assert(offset.size() == params.spans.size());
    gsl::not_null<uint8_t*> ptr = static_cast<uint8_t*>(srcPtr.get());
    for(size_t i = 0; i < offset.size(); ++i)
    {
    }
    return ptr;
}

}
