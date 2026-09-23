#include "CollisionDetector.h"

bool CollisionDetector::collision(const sf::Sprite& object1, const sf::Sprite& object2) {
	OrientedBoundingBox OBB1(object1);
	OrientedBoundingBox OBB2(object2);

	// Create the four distinct axes that are perpendicular to the edges of the two rectangles
	sf::Vector2f Axes[4] = {
		sf::Vector2f(OBB1.Points[1].x - OBB1.Points[0].x,
		OBB1.Points[1].y - OBB1.Points[0].y),
		sf::Vector2f(OBB1.Points[1].x - OBB1.Points[2].x,
		OBB1.Points[1].y - OBB1.Points[2].y),
		sf::Vector2f(OBB2.Points[0].x - OBB2.Points[3].x,
		OBB2.Points[0].y - OBB2.Points[3].y),
		sf::Vector2f(OBB2.Points[0].x - OBB2.Points[1].x,
		OBB2.Points[0].y - OBB2.Points[1].y)
	};

	for (int i = 0; i<4; i++) // For each axis...
	{
		float MinOBB1, MaxOBB1, MinOBB2, MaxOBB2;

		// ... project the points of both OBBs onto the axis ...
		OBB1.ProjectOntoAxis(Axes[i], MinOBB1, MaxOBB1);
		OBB2.ProjectOntoAxis(Axes[i], MinOBB2, MaxOBB2);

		// ... and check whether the outermost projected points of both OBBs overlap.
		// If this is not the case, the Seperating Axis Theorem states that there can be no collision between the rectangles
		if (!((MinOBB2 <= MaxOBB1) && (MaxOBB2 >= MinOBB1)))
			return false;
	}

	return true;	// Collision detected!
}

using TextureMask = std::vector<std::uint8_t>;

static std::uint8_t getPixel(const TextureMask& mask, const sf::Texture& tex, uint32_t x, uint32_t y) {
	if (x > tex.getSize().x || y > tex.getSize().y)
		return 0;

	return mask[x + y * tex.getSize().x];
}

class BitmaskRegistry
{
public:
	auto& create(const sf::Texture& tex, const sf::Image& img) {
		auto mask = TextureMask(tex.getSize().y * tex.getSize().x);

		for (uint32_t y = 0; y < tex.getSize().y; ++y)
		{
			for (uint32_t x = 0; x < tex.getSize().x; ++x)
				
				mask[x + y * tex.getSize().x] = img.getPixel(sf::Vector2u{ x, y }).a;
		}

		// store and return ref to the mask
		return (bitmasks[&tex] = std::move(mask));
	}

	auto& get(const sf::Texture& tex) {
		auto pair = bitmasks.find(&tex);
		if (pair == bitmasks.end())
		{
			return create(tex, tex.copyToImage());
		}

		return pair->second;
	}

	
private:
	std::map<const sf::Texture*, TextureMask> bitmasks;
};

// Gets global instance of BitmaskRegistry.
// "static" to make sure this function doesn't leak to other source file
static BitmaskRegistry& bitmasks() {
	static BitmaskRegistry instance;
	return instance;
}


bool CollisionDetector::pixelPerfectTest(const sf::Sprite& sprite1, const sf::Sprite& sprite2, std::uint8_t alphaLimit) {
	// 1. Check for AABB intersection using the new findIntersection API
	auto intersectionOpt = sprite1.getGlobalBounds().findIntersection(sprite2.getGlobalBounds());

	if (!intersectionOpt.has_value())
		return false;

	sf::FloatRect intersection = intersectionOpt.value();

	// SFML 3 uses Vector2f for sizes and positions in Rects
	auto s1SubRect = sprite1.getTextureRect();
	auto s2SubRect = sprite2.getTextureRect();

	auto& mask1 = bitmasks().get(sprite1.getTexture());
	auto& mask2 = bitmasks().get(sprite2.getTexture());

	// Loop through the intersection area
	for (float i = intersection.position.x; i < intersection.position.x + intersection.size.x; ++i) {
		for (float j = intersection.position.y; j < intersection.position.y + intersection.size.y; ++j) {

			// transformPoint now takes a single sf::Vector2f
			sf::Vector2f worldPoint{ i, j };
			sf::Vector2f s1v = sprite1.getInverseTransform().transformPoint(worldPoint);
			sf::Vector2f s2v = sprite2.getInverseTransform().transformPoint(worldPoint);

			// Check if coordinates are within the valid sub-rect area
			if (s1v.x >= 0 && s1v.y >= 0 && s2v.x >= 0 && s2v.y >= 0 &&
				s1v.x < s1SubRect.size.x && s1v.y < s1SubRect.size.y &&
				s2v.x < s2SubRect.size.x && s2v.y < s2SubRect.size.y) {

			


				// getPixel in SFML 3 also uses sf::Vector2u/f
				if (getPixel(mask1, sprite1.getTexture(),
					static_cast<unsigned>(s1v.x) + s1SubRect.position.x,
						static_cast<unsigned>(s1v.y) + s1SubRect.position.y) > alphaLimit &&
					getPixel(mask2, sprite2.getTexture(),
						static_cast<unsigned>(s2v.x) + s2SubRect.position.x,
							static_cast<unsigned>(s2v.y) + s2SubRect.position.y) > alphaLimit) {
					return true;
				}
			}
		}
	}
	return false;
}
