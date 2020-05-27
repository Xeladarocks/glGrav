

#define G 0.04f
#define SOFTENING 0

class Body {
public:
	glm::vec3 pos;
	glm::vec3 vel;
	float mass;
	float radius;
	glm::vec3 color;

	Body(glm::vec3 pos, glm::vec3 vel, float mass, float radius, glm::vec3 color) : pos(pos), vel(vel), mass(mass), radius(radius), color(color) {};
	Body(glm::vec3 pos, glm::vec3 vel, float mass, glm::vec3 color) : pos(pos), vel(vel), mass(mass), radius(std::sqrt(G* mass) / 50.f), color(color) {};
	Body(glm::vec3 pos, glm::vec3 vel, float mass, float radius) : pos(pos), vel(vel), mass(mass), radius(radius), color(glm::vec3(randf(), randf(), randf())) {};
	Body(glm::vec3 pos, glm::vec3 vel, float mass) : pos(pos), vel(vel), mass(mass), radius(std::sqrt(G* mass) / 50.f), color(glm::vec3(randf(), randf(), randf())) {};

	void tick(Body &other, float dt) {
		glm::vec3 a0 = this->getAcceleration(other);
		this->vel = this->vel + (a0 * 0.5f * dt);
		this->pos = this->pos + (this->vel * dt);
		glm::vec3 a1 = this->getAcceleration(other);
		this->vel = this->vel + (a1 * 0.5f * dt);
	}
	glm::vec3 getAcceleration(Body &other) {
		glm::vec3 posDiff;
		posDiff.x = (this->pos.x - other.pos.x);
		posDiff.y = (this->pos.y - other.pos.y);
		posDiff.z = (this->pos.z - other.pos.z);
		float dist = glm::length(posDiff);
		if (dist == 0) return glm::vec3(0);

		float F = (G * this->mass * other.mass) / (float)(dist * dist); // * time_step

		posDiff = -glm::normalize(posDiff);
		glm::vec3 new_accel(0);
		new_accel.x = F * posDiff.x / this->mass;
		new_accel.y = F * posDiff.y / this->mass;
		new_accel.z = F * posDiff.z / this->mass;

		return new_accel;
	}
};