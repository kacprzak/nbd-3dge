#include "Animation.h"

#include <glm/glm.hpp>

namespace gfx {

std::pair<int, int> Animation::Sampler::findKeyFrames(float time,
                                                      const std::vector<float>& input) const
{
    auto it       = std::upper_bound(input.cbegin(), input.cend(), time);
    int nextIdx   = std::distance(input.cbegin(), it);
    int prevIndex = nextIdx - 1;

    // If animation is not starting at time 0 then set it on first keyFrame
    prevIndex = std::max(prevIndex, 0);

    return {prevIndex, nextIdx};
}

//------------------------------------------------------------------------------

template <typename T>
void Animation::Sampler::lookup(float time, T* result, std::size_t size) const
{
    const auto& in = input.getData<float>();
    time           = glm::mod(time, in.back());

    auto keyFrames = findKeyFrames(time, in);

    const float prevFrameTime = in.at(keyFrames.first);
    const float nextFrameTime = in.at(keyFrames.second);
    const float dt            = nextFrameTime - prevFrameTime;
    const float t             = dt == 0.0f ? 0.0f : (time - prevFrameTime) / dt;

    const auto kfd = keyFrames.second - keyFrames.first; // keyframes distance. Typically 1 or 0

    T v0, b0{}, a1{}, v1{};

    if (interpolation == Step) {
        const auto& values =
            output.getElements<T>(keyFrames.first * size, (keyFrames.first + 1) * size);

        for (std::size_t i = 0u; i < size; ++i) {
            v0        = values[i];
            result[i] = interpolate(v0, b0, a1, v1, dt, t);
        }
    } else if (interpolation == CubicSpline) {
        // +1 -1 to not load in-tangen 0 and out-tangent 1
        const auto& values = output.getElements<T>((keyFrames.first * 3) * size + 1,
                                                   ((keyFrames.second + 1) * 3) * size - 1);
        for (std::size_t i = 0u; i < size; ++i) {
            v0 = values[i * 3];
            v1 = values[(i + kfd * size) * 3];
            if (kfd > 0) {
                b0 = values[i * 3 + 1];
                a1 = values[(i + kfd * size) * 3 - 1];
            }
            result[i] = interpolate(v0, b0, a1, v1, dt, t);
        }
    } else /* Linear */ {
        const auto& values =
            output.getElements<T>(keyFrames.first * size, (keyFrames.second + 1) * size);

        for (std::size_t i = 0u; i < size; ++i) {
            v0        = values[i];
            v1        = values[i + kfd * size];
            result[i] = interpolate(v0, b0, a1, v1, dt, t);
        }
    }
} // namespace gfx

//------------------------------------------------------------------------------

template <typename T>
T Animation::Sampler::interpolate(T v0, T b0, T a1, T v1, float dt, float t) const
{
    if (interpolation == Step || dt == 0.0f) {
        return v0;

    } else if (interpolation == CubicSpline) {
        auto p0 = v0;      // point 0
        auto m0 = b0 * dt; // out-tangent 0
        auto p1 = v1;      // point 1
        auto m1 = a1 * dt; // in-tangent 1
        // Hermite interpolation
        float t2 = t * t;
        float t3 = t2 * t;
        return (2 * t3 - 3 * t2 + 1) * p0 + (t3 - 2 * t2 + t) * m0 + (-2 * t3 + 3 * t2) * p1 +
               (t3 - t2) * m1;

    } else /* Linear */ {
        return glm::mix(v0, v1, t);
    }
}

//==============================================================================

void Animation::update(float delta, std::vector<Node>& nodes)
{
    for (const auto& channel : m_channels) {
        Node& node = nodes.at(channel.node);

        progress += delta;

        switch (channel.path) {
        case Channel::Translation: {
            glm::vec3 translation;
            channel.sampler.lookup(progress, &translation);
            node.setTranslation(translation);
        } break;
        case Channel::Rotation: {
            glm::quat rotation;
            channel.sampler.lookup(progress, &rotation);
            node.setRotation(glm::normalize(rotation));
        } break;
        case Channel::Scale: {
            glm::vec3 scale;
            channel.sampler.lookup(progress, &scale);
            node.setScale(scale);
        } break;
        case Channel::Weights: {
            auto weightsSize = node.getWeightsSize();
            std::vector<float> weights(weightsSize);
            channel.sampler.lookup(progress, weights.data(), weights.size());
            node.setWeights(weights);
        } break;
        }
    }
}

} // namespace gfx
