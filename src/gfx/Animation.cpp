#include "Animation.h"

#include <glm/glm.hpp>

namespace gfx {

template <typename T>
T Animation::Sampler::lookup(float time) const
{
    auto in = input.getData<float>();

    float animTime = glm::mod(time, in.back());
    // todo: change to binary search
    auto it = std::find_if(in.cbegin(), in.cend(), [animTime](float x) { return x > animTime; });
    std::size_t nextKeyFrameIdx = std::distance(in.cbegin(), it);

    float nextFrameTime = in.at(nextKeyFrameIdx);
    float prevFrameTime = in.at(nextKeyFrameIdx - 1);

    auto out = output.getData<T>();

    auto nextFrameValue = out.at(nextKeyFrameIdx);
    auto prevFrameValue = out.at(nextKeyFrameIdx - 1);

    float progress = (animTime - prevFrameTime) / (nextFrameTime - prevFrameTime);

    return glm::mix(prevFrameValue, nextFrameValue, progress);
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
            // todo
        } break;
        }
    }
}

} // namespace gfx
