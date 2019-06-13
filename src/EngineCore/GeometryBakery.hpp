#ifndef GeometryBakery_hpp
#define GeometryBakery_hpp

#include <memory>
#include <tuple>
#include <vector>

#include "GenericVertexLayout.hpp"
#include "types.hpp"

typedef std::vector<std::vector<uint8_t>> VertexData;
typedef std::vector<uint32_t>             IndexData;

typedef std::shared_ptr<VertexData>          VertexDataPtr;
typedef std::shared_ptr<IndexData>           IndexDataPtr;
typedef std::shared_ptr<GenericVertexLayout> VertexLayoutPtr;

namespace EngineCore
{
    namespace Graphics
    {

        /**
        * \brief Creates and return triangle geometry
        */
        std::tuple<VertexData, IndexData, GenericVertexLayout> createTriangle();

        /**
        * \brief Creates and return plane (quad) geometry
        */
        std::tuple<VertexDataPtr, IndexDataPtr, VertexLayoutPtr> createPlane(float width, float height);

        /**
        * \brief Creates and returns unit box geometry
        */
        std::tuple<VertexDataPtr, IndexDataPtr, VertexLayoutPtr> createBox();

        /**
        * \brief Create an ico sphere mesh
        * \param subdivisions Control the subdivions of the sphere
        * \return Returns shared pointer to the mesh
        */
        std::tuple<VertexData, IndexData, GenericVertexLayout> createIcoSphere(uint subdivions);
    }
}

#endif // !GeometryBakery_hpp
