#include "Simulation.h"
#include "Vendor/FastNoiseLite.h"

void Simulation::Init() {

	// Allocating the Map
	Map = new TileComponent * [MapSize];
	for (int i = 0;i < MapSize;++i) {
		Map[i] = new TileComponent[MapSize];
	}

	// Generating Food distribution
	FastNoiseLite Noise;
	Noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);

	for (int y = 0;y < MapSize;++y) {
		for (int x = 0;x < MapSize;++x) {
			short v = (std::pow(((Noise.GetNoise((float)x, (float)y) + 1)/2),6) * MAX_ENERGY_IN_GENERATED_TILE);
			Map[x][y].Energy = v;
			Map[x][y].MaxEnergy = v;
		}
	}
}

void Simulation::UpdateSimulation() {

	// Makes the energy in tiles regrow
	for (int i = 0;i < MapSize;++i) {
		for (int j = 0;j < MapSize;++j) {
			Map[i][j].Energy += TILE_GROWTH * (Map[i][j].Energy < Map[i][j].MaxEnergy);
			Map[i][j].Energy = Map[i][j].Energy + (Map[i][j].Energy > Map[i][j].MaxEnergy) * (Map[i][j].MaxEnergy - Map[i][j].Energy);
		}
	}

	for (int i = Agents.size();i < POPULATION_MINIMUM;++i) {
		sf::Vector2i Positon;
		do {
			Positon.x = rand() % 45;
			Positon.y = rand() % 45;
		} while (Map[Positon.x + MapSize / 2][Positon.y + MapSize / 2].Agent != nullptr);
		AddAgent(GenerateColor(), Positon);
	}

	std::vector<Agent*> AgentsToDestroy;

	// Updating the agents
	MutexRenderUpdateLoop.lock();
	int AgentCount = Agents.size();
	for (int i = 0; i < AgentCount;i++) {
		float* Input = CompileAgentInput(Agents[i]);
		float* Out = Agents[i]->Update(Input);

		Agents[i]->Energy--;

		float Left = Out[LEFT_NEURON];
		float Right = Out[RIGHT_NEURON];
		float Up = Out[UP_NEURON];
		float Down = Out[DOWN_NEURON];
		float Eat = Out[EAT_NEURON];
		float Reproduce = Out[REPRODUCE_NEURON];

		if (Left >= Right && Left >= Up && Left >= Down && Left > 0)
			Agents[i]->Energy -= WALKING_COST * SetAgentPosition(Agents[i], Agents[i]->Position + sf::Vector2i(-1, 0));
		else if (Right >= Up && Right >= Down && Right > 0)
			Agents[i]->Energy -= WALKING_COST * SetAgentPosition(Agents[i], Agents[i]->Position + sf::Vector2i(1, 0));
		else if (Up >= Down && Up > 0)
			Agents[i]->Energy -= WALKING_COST * SetAgentPosition(Agents[i], Agents[i]->Position + sf::Vector2i(0, -1));
		else if (Down > 0)
			Agents[i]->Energy -= WALKING_COST * SetAgentPosition(Agents[i], Agents[i]->Position + sf::Vector2i(0, 1));
		else if (Reproduce > 0) {
			Agents[i]->Energy -= BIRTH_COST * AddAgentFromCopy(Agents[i]);
		}
		else if (Eat > 0) {
			AgentConsumesEnergy(Agents[i]);
		}
		if (Agents[i]->Energy < AGENT_MINIMUM_ENERGY || Agents[i]->Energy > AGENT_MAXIMUM_ENERGY + 20) AgentsToDestroy.push_back(Agents[i]);
	}

	int AgentDestroyCount = AgentsToDestroy.size();
	for (int i = 0;i < AgentDestroyCount;++i) {
		DestroyAgent(AgentsToDestroy[i]);
	}
	MutexRenderUpdateLoop.unlock();
}

bool Simulation::SetAgentPosition(Agent* agent, sf::Vector2i Position) {
	int x = Position.x + MapSize / 2;
	int y = Position.y + MapSize / 2;
	bool MovmentAllowed = -1 < x && x < MapSize && -1 < y < MapSize && Map[x][y].Agent == nullptr;
	if (MovmentAllowed) {
		Map[agent->Position.x + MapSize/2][agent->Position.y + MapSize/2].Agent = nullptr;
		agent->SetPosition(Position);
		Map[x][y].Agent = agent;
	}
	return MovmentAllowed;
}

void Simulation::AgentConsumesEnergy(Agent* agent) {
	int x = agent->Position.x + MapSize / 2;
	int y = agent->Position.y + MapSize / 2;
	bool FullHarvest = Map[x][y].Energy >= ENERGY_ABSORPTION_RATE;
	agent->Energy += ENERGY_ABSORPTION_RATE * FullHarvest;
	Map[x][y].Energy -= ENERGY_ABSORPTION_RATE * FullHarvest;
	agent->Energy += Map[x][y].Energy * !FullHarvest;
	Map[x][y].Energy -= Map[x][y].Energy * !FullHarvest;
}

void Simulation::AddAgent(sf::Color Color, sf::Vector2i Position) {
	Agent* agent = new Agent(Color, Position);
	agent->Energy = AGENT_STARTING_ENERGY;
    agent->NNController->RandomizeByChance(INITIAL_MUTATION_CHANCE, 0.2f);
	int x = agent->Position.x + MapSize / 2;
	int y = agent->Position.y + MapSize / 2;
	Map[x][y].Agent = agent;
	Agents.push_back(agent);
}

bool Simulation::AddAgentFromCopy(Agent* agent) {
	int x = agent->Position.x + MapSize / 2;
	int y = agent->Position.y + MapSize / 2;

	bool Left = (x - 1 > -1 ? Map[x - 1][y].Agent == nullptr : false);
	bool Right = (x + 1 < MapSize ? Map[x + 1][y].Agent == nullptr : false);
	bool Up = (y - 1 > -1 ? Map[x][y - 1].Agent == nullptr : false);
	bool Down = (y + 1 < MapSize ? Map[x][y + 1].Agent == nullptr : false);

	if (!Left && !Right && !Up && !Down) return false;

	Agent* Copy = new Agent(*agent);

	if (Left) {
		Copy->SetPosition(agent->Position + sf::Vector2i(-1, 0));
	}
	else if (Right) {
		Copy->SetPosition(agent->Position + sf::Vector2i(1, 0));
	}
	else if (Up) {
		Copy->SetPosition(agent->Position + sf::Vector2i(0, -1));
	}
	else if (Down) {
		Copy->SetPosition(agent->Position + sf::Vector2i(0, 1));
	}

	if (rand() < RAND_MAX * CHANCE_OF_MUTATED_COPY) {
		Copy->NNController->RandomizeByChance(MUTATION_CHANCE_ON_COPY, MUTATION_APLITUDE_ON_COPY);
		if (rand() < RAND_MAX * COLOR_CHANGE_VALUE) {
			Copy->Color = sf::Color(GenerateColor() * Copy->Color);
		}
	}

	Map[Copy->Position.x + MapSize / 2][Copy->Position.y + MapSize / 2].Agent = Copy;
	Agents.push_back(Copy);
	return true;
}

void Simulation::DestroyAgent(Agent* agent) {
	Agents.erase(std::find(Agents.begin(), Agents.end(), agent));
	delete agent;
}

int Simulation::GetWorldArea() {
	return MapSize * MapSize;
}

void Simulation::SetTileToAgentData(Agent* agent) {
	int x = agent->Position.x + MapSize / 2;
	int y = agent->Position.y + MapSize / 2;

}

sf::Color Simulation::GenerateColor() {
	return sf::Color::Color(rand() % 200 + 55, rand() % 200 + 55, rand() % 200 + 55);
}

TileData Simulation::GetTileDataAt(int x, int y) {
	x += MapSize / 2;
	y += MapSize / 2;
	TileData tileData;
	if (Map[x][y].Agent != nullptr) {
		tileData.R = Map[x][y].Agent->Color.r / 255.0f;
		tileData.G = Map[x][y].Agent->Color.g / 255.0f;
		tileData.B = Map[x][y].Agent->Color.b / 255.0f;
		tileData.Signal = Map[x][y].Agent->CurrentSignalState;
		tileData.AgentAttacks = Map[x][y].Agent->Attacks;
		tileData.AgentIsAttacked = Map[x][y].Agent->IsGettingAttacked;
	}
	else {
		tileData.R = -1;
		tileData.G = -1;
		tileData.B = -1;
		tileData.Signal = 0;
		tileData.AgentAttacks = 0;
		tileData.AgentIsAttacked = 0;
	}
	tileData.Energy = Map[x][y].Energy;
	tileData.EnergyCapacity = Map[x][y].MaxEnergy;
	return tileData;
}

float* Simulation::CompileAgentInput(Agent* agent) {

	// RGBSAA|RGBSAA|RGBSAA|...|RGBSAA|OwnSignalIn|UnderTileEnergy|Memory(30 values)|

	int index = 0;
	const int ViewRange = 4;

	static float Input[AGENT_INPUT_SIZE];

	for (int i = agent->Position.x - 4;i <= agent->Position.x + 4;++i) {
		for (int j = agent->Position.y - 4;j <= agent->Position.y + 4;++j) {
			TileData Data = GetTileDataAt(i,j);
			Input[index++] = Data.R; //R
			Input[index++] = Data.G; //G
			Input[index++] = Data.B; //B
			Input[index++] = 1 * Data.Signal + 0 * !Data.Signal; //S
			Input[index++] = 1 * Data.AgentAttacks + -1 * !Data.AgentAttacks;
			Input[index++] = 1 * Data.AgentIsAttacked + -1 * !Data.AgentIsAttacked;
		}
	}

	Input[index++] = agent->CurrentSignalState;

	Input[index++] = (agent->Energy / AGENT_MAXIMUM_ENERGY) * 2 - 1;

	TileData UnderTile = GetTileDataAt(agent->Position.x, agent->Position.y);
	Input[index++] = (UnderTile.Energy / (float)MAX_ENERGY_IN_GENERATED_TILE) * 2 - 1;

	for (int i = 0;i < AGENT_MEMORY_SIZE;++i) {
		Input[index++] = agent->Memory[i];
	}

	return Input;

}