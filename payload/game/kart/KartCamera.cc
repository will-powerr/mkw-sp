#include "Common.hh"
#include "game/kart/KartObjectProxy.hh"
#include "game/system/InputManager.hh"
extern "C" {
#include <revolution.h>
}

constexpr f32 YAW_TAN = 2.0f;
constexpr f32 PITCH_TAN = 0.7f;
constexpr f32 CLASSIC_STICK_RANGE = 320.0f;

Vec3 Normalized(const Vec3 &v) {
    f32 norm = Vec3::Norm(v);
    return norm > 0.0f ? v * (1.0f / norm) : v;
}

void CosSinFromTan(f32 t, f32 &c, f32 &s) {
    f32 len = Vec3::Norm(Vec3(1.0f, t, 0.0f));
    c = 1.0f / len;
    s = t / len;
}

Vec3 RotateAroundAxis(const Vec3 &v, const Vec3 &a, f32 c, f32 s) {
    return v * c + cross(a, v) * s + a * (dot(a, v) * (1.0f - c));
}

extern "C" {

void KartCamera_ApplyStickMovement(Vec3 *orbitDir, const Vec3 *yawAxis, const Vec3 *pitchAxis, Kart::KartObjectProxy *proxy) {


    auto *padProxy = proxy->getPadProxy();
    if (!padProxy) {
        return;
    }
    const System::Pad *controller = padProxy->pad();
    if (!controller) {
        return;
    }

    float x = 0.0f, y = 0.0f;
    switch (controller->getControllerId()) {
        case Registry::Controller::Classic: {
            auto raw = System::WiiPad::RawRightStick();
            x = -raw.x;
            y = -raw.y;

            break;
        }
        case Registry::Controller::GameCube: {
            auto gc_inputs = static_cast<const System::GCPad *>(controller);
            x = -gc_inputs->substick().x;
            y = -gc_inputs->substick().y;
            break;
        }
        default:
            return;
    }

    f32 magnitude = Vec3::Norm(Vec3(x, y, 0.0f)); // clamp to unit circle

    if (magnitude <= 0.0f)
        return;

    f32 clamped = magnitude > 1.0f ? 1.0f : magnitude;
    f32 factor = (clamped) / (1.0f * magnitude);

    x *= factor;
    y *= factor;

    Vec3 up = Normalized(*yawAxis);
    Vec3 right = Normalized(*pitchAxis);

    f32 cosine;
    f32 sine;
    Vec3 dir = *orbitDir;

    CosSinFromTan(y * PITCH_TAN, cosine, sine);
    dir = RotateAroundAxis(dir, right, cosine, sine); // pitch camera
    CosSinFromTan(x * YAW_TAN, cosine, sine);
    dir = RotateAroundAxis(dir, up, cosine, sine); // yaw camera

    *orbitDir = dir;

}

}