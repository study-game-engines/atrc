#include <agz/tracer/core/light.h>
#include <agz-utils/misc.h>

AGZ_TRACER_BEGIN

class NativeSky : public EnvirLight
{
    FSpectrum top_;
    FSpectrum bottom_;

    real user_specified_power_;

    FSpectrum radiance_impl(const FVec3 &ref_to_light) const noexcept
    {
        const real cos_theta = math::clamp<real>(
            ref_to_light.normalize().z, -1, 1);
        
        const real s = (cos_theta + 1) / 2;
        return s * top_ + (1 - s) * bottom_;
    }

public:

    NativeSky(
        const FSpectrum &top, const FSpectrum &bottom,
        real user_specified_power)
    {
        top_    = top;
        bottom_ = bottom;
        user_specified_power_ = user_specified_power;
    }

    LightSampleResult sample(
        const FVec3 &ref, const Sample5 &sam) const noexcept override
    {
        const auto [dir, pdf] = math::distribution::uniform_on_sphere(sam.u, sam.v);

        return LightSampleResult(
            ref, emit_pos(ref, dir).pos, -dir, {}, radiance_impl(dir), pdf);
    }

    real pdf(const FVec3 &ref, const FVec3 &) const noexcept override
    {
        return math::distribution::uniform_on_sphere_pdf<real>;
    }

    LightEmitResult sample_emit(const Sample5 &sam) const noexcept override
    {
        const auto [dir, pdf_dir] = math::distribution
                                        ::uniform_on_sphere(sam.u, sam.v);

        const Vec2 disk_sam   = math::distribution
                                    ::uniform_on_unit_disk(sam.w, sam.r);
        const FCoord dir_coord = FCoord::from_z(dir);
        const FVec3 pos        = world_radius_ *
            (disk_sam.x * dir_coord.x + disk_sam.y * dir_coord.y - dir) + world_centre_;

        return LightEmitResult(
            pos, dir, dir.normalize(), {}, radiance_impl(-dir),
            1 / (PI_r * world_radius_ * world_radius_), pdf_dir);
    }

    LightEmitPDFResult emit_pdf(
        const FVec3 &pos, const FVec3 &dir, const FVec3 &nor) const noexcept override
    {
        const real pdf_dir = math::distribution::uniform_on_sphere_pdf<real>;
        const real pdf_pos = 1 / (PI_r * world_radius_ * world_radius_);
        return { pdf_pos, pdf_dir };
    }

    LightEmitPosResult emit_pos(
        const FVec3 &ref, const FVec3 &ref_to_light) const noexcept override
    {
        // o: world_center
        // r: world_radius
        // x: ref
        // d: ref_to_light.normalize()
        // o + r * (u * ex + v * ey + d) = x + d * t
        // solve [u, v, t] and ans = x + d * t

        // => [a b c][u v t]^T = m
        // where a = r * ex
        //       b = r * ey
        //       c = -d
        //       m = x - o + r * d

        const auto [ex, ey, d] = FCoord::from_z(ref_to_light);

        const FVec3 a = world_radius_ * ex;
        const FVec3 b = world_radius_ * ey;
        const FVec3 c = -d;
        const FVec3 m = ref - world_centre_ - world_radius_ * d;

        const real D0 = a[1] * b[2] - a[2] * b[1];
        const real D1 = a[0] * b[2] - a[2] * b[0];
        const real D2 = a[0] * b[1] - a[1] * b[0];

        const real det  = c[0] * D0 - c[1] * D1 + c[2] * D2;
        const real tdet = m[0] * D0 - m[1] * D1 + m[2] * D2;

        const real t = tdet / det;
        const FVec3 pos = ref + t * d;

        return { pos, c };
    }

    FSpectrum power() const noexcept override
    {
        if(user_specified_power_ > 0)
            return FSpectrum(user_specified_power_);
        const real radius = world_radius_;
        const FSpectrum mean_radiance = (top_ + bottom_) * real(0.5);
        return 4 * PI_r * PI_r * radius * radius * mean_radiance;
    }

    FSpectrum radiance(
        const FVec3 &ref, const FVec3 &ref_to_light) const noexcept override
    {
        return radiance_impl(ref_to_light);
    }
};

RC<EnvirLight> create_native_sky(
    const FSpectrum &top,
    const FSpectrum &bottom,
    real user_specified_power)
{
    return newRC<NativeSky>(top, bottom, user_specified_power);
}

AGZ_TRACER_END
