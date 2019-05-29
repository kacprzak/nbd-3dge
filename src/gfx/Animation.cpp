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

    float prevFrameTime = in.at(keyFrames.first);
    float nextFrameTime = in.at(keyFrames.second);
    float dt            = nextFrameTime - prevFrameTime;
    float t             = dt == 0.0f ? 0.0f : (time - prevFrameTime) / dt;

    T v0, b0{}, a1{}, v1{};

    if (interpolation == Step || t == 0.0f) {
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
            v0        = values[i * size];
            b0        = values[i * size + 1];
            a1        = values[i * size + 2];
            v1        = values[i * size + 3];
            result[i] = interpolate(v0, b0, a1, v1, dt, t);
        }
    } else /* Linear */ {
        const auto& values =
            output.getElements<T>(keyFrames.first * size, (keyFrames.second + 1) * size);

        for (std::size_t i = 0u; i < size; ++i) {
            v0        = values[i * size];
            v1        = values[i * size + 1];
            result[i] = interpolate(v0, b0, a1, v1, dt, t);
        }
    }
} // namespace gfx

//------------------------------------------------------------------------------

template <typename T>
void Animation::Sampler::lookupArray(float time, std::vector<T>& result) const
{
    const auto& in = input.getData<float>();
    time           = glm::mod(time, in.back());

    auto keyFrames = findKeyFrames(time, in);

    float prevFrameTime = in.at(keyFrames.first);
    float nextFrameTime = in.at(keyFrames.second);
    float dt            = nextFrameTime - prevFrameTime;
    float t             = dt == 0.0f ? 0.0f : (time - prevFrameTime) / dt;

    auto numOfElements = output.count / in.size();
    result.resize(numOfElements);

    if (interpolation == Step || keyFrames.first == keyFrames.second) {
        const auto& values = output.getElements<T>(keyFrames.first * numOfElements,
                                                   keyFrames.first * numOfElements + numOfElements);
        std::copy(std::cbegin(values), std::cend(values), std::begin(result));

    } else /* Linear */ {
        const auto& values = output.getElements<T>(
            keyFrames.first * numOfElements, keyFrames.second * numOfElements + numOfElements);

        // Zip
        for (auto i = 0u; i < numOfElements; ++i) {
            result[i] = glm::mix(values[i], values[numOfElements + i], t);
        }
    }
}

//------------------------------------------------------------------------------

template <typename T>
T Animation::Sampler::interpolate(T v0, T b0, T a1, T v1, float dt, float t) const
{
    if (interpolation == Step) {
        return v0;

    } else if (interpolation == CubicSpline) {
        auto p0 = v0;      // point 0
        auto m0 = dt * b0; // out-tangent 0
        auto p1 = v1;      // point 1
        auto m1 = dt * a1; // in-tangent 1
        // Herimte interpolation
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

        static float progress = 0.0f;
        progress += delta / 30.0f;

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
            std::vector<float> weights;
            channel.sampler.lookupArray(progress, weights);
            node.setWeights(weights);
        } break;
        }
    }
}

} // namespace gfx
