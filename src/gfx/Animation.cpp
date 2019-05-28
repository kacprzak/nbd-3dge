#include "Animation.h"

#include <glm/glm.hpp>

namespace gfx {

std::pair<int, int> Animation::Sampler::findKeyFrames(float time,
                                                      const std::vector<float>& input) const
{
    auto it             = std::upper_bound(input.cbegin(), input.cend(), time);
    std::size_t nextIdx = std::distance(input.cbegin(), it);
    return {nextIdx - 1, nextIdx};
}

//------------------------------------------------------------------------------

template <typename T>
T Animation::Sampler::lookup(float time) const
{
    const auto& in = input.getData<float>();
    time           = glm::mod(time, in.back());

    auto keyFrames = findKeyFrames(time, in);

    float prevFrameTime = in.at(keyFrames.first);
    float nextFrameTime = in.at(keyFrames.second);

    float progress = (time - prevFrameTime) / (nextFrameTime - prevFrameTime);

    const auto& values = output.getElements<T>(keyFrames.first, keyFrames.second + 1);

    return glm::mix(values[0], values[1], progress);
}

//------------------------------------------------------------------------------

template <typename T>
void Animation::Sampler::lookupArray(float time, std::vector<T>& result) const
{
    const auto& in = input.getData<float>();
    time           = glm::mod(time, in.back());

    auto keyFrames = findKeyFrames(time, in);

    float prevFrameTime = in.at(keyFrames.first);
    float nextFrameTime = in.at(keyFrames.second);

    float progress = (time - prevFrameTime) / (nextFrameTime - prevFrameTime);

    auto numOfElements = output.count / in.size();

    const auto& values = output.getElements<T>(keyFrames.first * numOfElements,
                                               keyFrames.second * numOfElements + numOfElements);

    result.resize(numOfElements);
    // Zip
    for (auto i = 0u; i < numOfElements; ++i) {
        result[i] = glm::mix(values[i], values[numOfElements + i], progress);
    }
}

//==============================================================================

void Animation::update(float delta, std::vector<Node>& nodes)
{
    for (const auto& channel : m_channels) {
        Node& node = nodes.at(channel.node);

        static float progress = 0.0f;
        progress += delta;

        switch (channel.path) {
        case Channel::Translation: {
            auto sample = channel.sampler.lookup<glm::vec3>(progress);
            node.setTranslation(sample);
        } break;
        case Channel::Rotation: {
            auto sample = channel.sampler.lookup<glm::quat>(progress);
            node.setRotation(sample);
        } break;
        case Channel::Scale: {
            auto sample = channel.sampler.lookup<glm::vec3>(progress);
            node.setScale(sample);
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
