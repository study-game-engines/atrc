#include <Atrc/Entity/GlassSphere.h>
#include <Atrc/Material/BxDF.h>
#include <Atrc/Math/Math.h>

AGZ_NS_BEG(Atrc)

namespace
{
    bool Refract(const Vec3r &wi, const Vec3r &nor, Real niDivNt, Vec3r *refracted)
    {
        Real t = -Dot(wi, nor);
        Real jdg = Real(1) - niDivNt * niDivNt * (1 - t * t);
        if(jdg > Real(0))
        {
            *refracted = niDivNt * (-wi - t * nor) - Sqrt(jdg) * nor;
            return true;
        }
        return false;
    }

    Real ChristopheSchlick(Real cos, Real refIdx)
    {
        Real t = (1 - refIdx) / (1 + refIdx);
        t = t * t;
        return t + (1 - t) * Pow(1 - cos, 5);
    }

    class GlassBxDF
        : ATRC_IMPLEMENTS BxDF,
          ATRC_PROPERTY AGZ::Uncopiable
    {
        CoordSys local_;
        Spectrum reflColor_, refrColor_;
        Real refIdx_;

    public:

        explicit GlassBxDF(
            const Intersection &inct, const Spectrum &reflColor, const Spectrum &refrColor, Real refIdx)
            : local_(CoordSys::FromZ(inct.nor)), reflColor_(reflColor), refrColor_(refrColor), refIdx_(refIdx)
        {
            
        }

        BxDFType GetType() const override
        {
            return BXDF_REFLECTION | BXDF_TRANSMISSION | BXDF_SPECULAR;
        }

        Spectrum Eval(const Vec3r &wi, const Vec3r &wo) const override
        {
            return SPECTRUM::BLACK;
        }

        Option<BxDFSample> Sample(const Vec3r &wi, BxDFType type) const override
        {
            Real dot = Dot(wi, local_.ez), absDot = Abs(dot);
            Real niDivNt, cosine = absDot;

            Vec3r nor(AGZ::UNINITIALIZED);
            if(dot < 0)
            {
                niDivNt = refIdx_;
                nor = -local_.ez;
                cosine *= refIdx_;
            }
            else
            {
                niDivNt = 1 / refIdx_;
                nor = local_.ez;
            }

            if(type.Contains(BXDF_TRANSMISSION | BXDF_SPECULAR))
            {
                if(!type.Contains(BXDF_REFLECTION) ||
                   Rand() > ChristopheSchlick(cosine, refIdx_))
                {
                    Vec3r refrDir;
                    if(Refract(wi, nor, niDivNt, &refrDir))
                    {
                        BxDFSample ret;
                        ret.dir = refrDir;
                        ret.coef = refrColor_ / SS(absDot);
                        ret.pdf = 1;
                        return ret;
                    }
                }
            }

            if(type.Contains(BXDF_REFLECTION | BXDF_SPECULAR))
            {
                BxDFSample ret;
                ret.dir = 2 * absDot * nor - wi;
                ret.coef = reflColor_ / SS(absDot);
                ret.pdf = 1;
                return ret;
            }

            return None;
        }
    };
}

GlassSphere::GlassSphere(
    Real radius, const Transform &local2World,
    const Spectrum &reflColor, const Spectrum &refrColor, Real refIdx)
    : Sphere(radius, local2World),
      reflectedColor_(reflColor), refractedColor_(refrColor), refIdx_(refIdx)
{
    
}

RC<BxDF> GlassSphere::GetBxDF(const Intersection &inct) const
{
    return NewRC<GlassBxDF>(inct, reflectedColor_, refractedColor_, refIdx_);
}

AGZ_NS_END(Atrc)
