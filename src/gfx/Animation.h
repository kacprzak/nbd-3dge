#ifndef GFX_ANIMATION_H
#define GFX_ANIMATION_H

#include "Buffer.h"
#include "Node.h"

namespace gfx {

class Animation final
{
  public:
    struct Sampler
    {
        enum Interpolation { Linear, Step, CubicSpline };

        Interpolation interpolation;
        Accessor input;
        Accessor output;

        std::pair<int, int> findKeyFrames(float time, const std::vector<float>& input) const;

        template <typename T>
        T lookup(float time) const;

        template <typename T>
        void lookupArray(float time, std::vector<T>& result) const;
    };

    struct Channel
    {
        enum Path { Translation, Rotation, Scale, Weights };

        int node = -1;
        Path path;
        Sampler sampler;
    };

    Animation(const std::vector<Channel>& channels)
        : m_channels{channels}
    {
    }

    void update(float delta, std::vector<Node>& nodes);

  private:
    std::vector<Channel> m_channels;
};

} // namespace gfx

#endif // GFX_ANIMATION_H
