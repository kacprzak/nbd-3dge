#include "Animation.h"

#include <glm/glm.hpp>

namespace gfx {

std::pair<int, int> Animation::Sampler::findKeyFrames(float time,
                                                      const std::vector<float>& input) const
{
    // todo: change to binary search
    auto it = std::find_if(input.cbegin(), input.cend(), [time](float x) { return x > time; });
    std::size_t nextKeyFrameIdx = std::distance(input.cbegin(), it);
    return {nextKeyFrameIdx - 1, nextKeyFrameIdx};
}

//------------------------------------------------------------------------------

template <typename T>
T Animation::Sampler::lookup(float time) const
{
    auto in = input.getData<float>();
    time    = glm::mod(time, in.back());

    auto keyFrames = findKeyFrames(time, in);

    float prevFrameTime = in.at(keyFrames.first);
    float nextFrameTime = in.at(keyFrames.second);

    float progress = (time - prevFrameTime) / (nextFrameTime - prevFrameTime);

    auto out = output.getData<T>();

    auto prevFrameValue = out.at(keyFrames.first);
    auto nextFrameValue = out.at(keyFrames.second);

    return glm::mix(prevFrameValue, nextFrameValue, progress);
}

//------------------------------------------------------------------------------

template <typename T>
void Animation::Sampler::lookupArray(float time, std::vector<T>& result) const
{
    auto in = input.getData<float>();
    time    = glm::mod(time, in.back());

    auto keyFrames = findKeyFrames(time, in);

    float prevFrameTime = in.at(keyFrames.first);
    float nextFrameTime = in.at(keyFrames.second);

    float progress = (time - prevFrameTime) / (nextFrameTime - prevFrameTime);

    auto out = output.getData<T>();

    auto numOfElements = out.size() / in.size();

    for (auto i = 0u; i < numOfElements; ++i) {
        auto prevFrameValue = out.at(keyFrames.first * numOfElements + i);
        auto nextFrameValue = out.at(keyFrames.second * numOfElements + i);
        result.push_back(glm::mix(prevFrameValue, nextFrameValue, progress));
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