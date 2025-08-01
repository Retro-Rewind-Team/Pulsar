#ifndef _NW4R_LYTPICTURE_
#define _NW4R_LYTPICTURE_
#include <types.hpp>
#include <core/nw4r/lyt/Pane.hpp>
#include <core/nw4r/lyt/common.hpp>
#include <core/nw4r/lyt/TexMap.hpp>

namespace nw4r {
namespace lyt {
class Picture : public Pane {
   public:
    Picture(const res::TextBox* src, const ResBlockSet& resBlockSet);  // 8007ad90
    ~Picture() override;  // 0x8 8007aee0 vtable 80273500
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;  // 0xc 8007b300
    void DrawSelf(const DrawInfo& info) override;  // 0x18 8007b220
    ut::Color GetVtxColor(u64 id) const override;  // 0x24 8007b180
    void SetVtxColor(u32 id, ut::Color color) override;  // 0x28 8007b1b0
    u8 GetVtxColorElement(u64 id) const override;  // 0x34 8007b1e0
    void SetVtxColorElement(u64 id, u8 value) override;  // 0x38 8007b200
    virtual void Append(const TexMap& texMap);  // 8007afc0
    ut::Color vertexColours[4];  // 0xd8
    detail::TexCoordAry texCoords;  // 0xe8
};  // Total size 0xf0
}  // namespace lyt
}  // namespace nw4r
#endif