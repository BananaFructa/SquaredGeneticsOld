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
			short v = (std::pow(((Noise.GetNoise((float)x, (float)y) + 1)/2),FOOD_RARITY) * MAX_ENERGY_IN_GENERATED_TILE);
			Map[x][y].Energy = v;
			Map[x][y].MaxEnergy = v;
		}
	}
}

void Simulation::UpdateSimulation() {

	static FFNN* BestNetwork;
	static int BestScore = 0;

	// Makes the energy in tiles regrow
	for (int i = 0;i < MapSize;++i) {
		for (int j = 0;j < MapSize;++j) {
			Map[i][j].Energy += TILE_GROWTH * (Map[i][j].Energy < Map[i][j].MaxEnergy);
			Map[i][j].Energy = Map[i][j].Energy + (Map[i][j].Energy > Map[i][j].MaxEnergy) * (Map[i][j].MaxEnergy - Map[i][j].Energy);
		}
	}

	if ((Agents.size() == 0 && USE_AUTOMATIC_SELECTION && BestScore == 0) || !USE_AUTOMATIC_SELECTION) {
		for (int i = Agents.size();i < POPULATION_MINIMUM;++i) {
			sf::Vector2i Positon;
			do {
				Positon.x = rand() % (MapSize)+(-MapSize / 2);
				Positon.y = rand() % (MapSize)+(-MapSize / 2);
			} while (Map[Positon.x + MapSize / 2][Positon.y + MapSize / 2].Agent != nullptr);
			AddAgent(GenerateColor(), Positon);
		}
	}

	else if (Agents.size() == 0 && USE_AUTOMATIC_SELECTION) {
		BestScore = 0;
		bool AddedFirst = false;
		for (int i = Agents.size();i < POPULATION_MINIMUM;++i) {
			sf::Vector2i Positon;
			do {
				Positon.x = rand() % (MapSize)+(-MapSize / 2);
				Positon.y = rand() % (MapSize)+(-MapSize / 2);
			} while (Map[Positon.x + MapSize / 2][Positon.y + MapSize / 2].Agent != nullptr);
			AddAgent(GenerateColor(), Positon);
			delete Agents[Agents.size() - 1]->NNController;
			Agents[Agents.size() - 1]->NNController = new FFNN(*BestNetwork);
			if (AddedFirst) {
				Agents[Agents.size() - 1]->NNController->RandomizeByChance(INITIAL_MUTATION_CHANCE, MUTATION_APLITUDE);
			}
			AddedFirst = true;
		}
	}

	std::vector<Agent*> AgentsToDestroy;

	MutexRenderUpdateLoop.lock();
	// Updating the agents
	int AgentCount = Agents.size();

	for (int i = 0;i < AgentCount;++i) {
		Agents[i]->AttackStateOfCurrentUpdate = false;
	}

	for (int i = 0; i < AgentCount;i++) {

		if (ALLOW_OVERCROUD_COST) PenaliseForPossibleNeighbourLimit(Agents[i]);

		// First check if agent should be dead
		if (Agents[i]->Energy < AGENT_MINIMUM_ENERGY || Agents[i]->Energy > AGENT_MAXIMUM_ENERGY + 20) {
			AgentsToDestroy.push_back(Agents[i]);
			continue;
		}

		Agents[i]->IsAttacked = Agents[i]->AttackStateOfCurrentUpdate;

		float* Input = CompileAgentInput(Agents[i]);
		float* Out = Agents[i]->Update(Input);

		for (int j = AGENT_OUTPUT_SIZE - AGENT_MEMORY_SIZE;j <= AGENT_OUTPUT_SIZE - 1;++j)
			Agents[i]->Memory[j - (AGENT_OUTPUT_SIZE - AGENT_MEMORY_SIZE)] = Out[j];

		Agents[i]->Energy -= AGENT_PASSIVE_COST;
		Agents[i]->CurrentSignalState = Out[SIGNAL_NEURON] > 0;

		float MoveH = Out[HORIZONTAL_MOVMENT_NEURON];
		float MoveV = Out[VERTICAL_MOVMENT_NEURON];
		float Eat = Out[EAT_NEURON];
		float Reproduce = Out[REPRODUCE_NEURON];
		float AttackX = Out[ATTACK_HORIZONTAL];
		float AttackY = Out[ATTACK_VERTICAL];

		int x = Agents[i]->Position.x + MapSize / 2;
		int y = Agents[i]->Position.y + MapSize / 2;


		if (Eat > 0) {
			AgentConsumesEnergy(Agents[i]);
		}

		bool AttackSucces = false;

		if (AttackX < -0.5f && std::abs(AttackX) > std::abs(AttackY)) {
			AttackSucces = AttackAtPosition(Agents[i]->Position + sf::Vector2i(-1, 0));
			Agents[i]->Energy -= ATTACK_COST * AttackSucces;
		}
		else if (AttackX > 0.5f && std::abs(AttackX) > std::abs(AttackY)) {
			AttackSucces = AttackAtPosition(Agents[i]->Position + sf::Vector2i(1, 0));
			Agents[i]->Energy -= ATTACK_COST * AttackSucces;
		}
		else if (AttackY < -0.5f) {
			AttackSucces = AttackAtPosition(Agents[i]->Position + sf::Vector2i(0, -1));
			Agents[i]->Energy -= ATTACK_COST * AttackSucces;
		}
		else if (AttackY > 0.5f) {
			AttackSucces = AttackAtPosition(Agents[i]->Position + sf::Vector2i(0, 1));
			Agents[i]->Energy -= ATTACK_COST * AttackSucces;
		}

		Agents[i]->Attacks = AttackSucces;

		if (MoveH < -0.5f && std::abs(MoveH) > std::abs(MoveV)) {
			Agents[i]->Energy -= WALKING_COST * SetAgentPosition(Agents[i], Agents[i]->Position + sf::Vector2i(-1, 0));
		}
		else if (MoveH > 0.5f && std::abs(MoveH) > std::abs(MoveV)) {
			Agents[i]->Energy -= WALKING_COST * SetAgentPosition(Agents[i], Agents[i]->Position + sf::Vector2i(1, 0));
		}
		else if (MoveV < -0.5f) {
			Agents[i]->Energy -= WALKING_COST * SetAgentPosition(Agents[i], Agents[i]->Position + sf::Vector2i(0, -1));
		}
		else if (MoveV > 0.5f) {
			Agents[i]->Energy -= WALKING_COST * SetAgentPosition(Agents[i], Agents[i]->Position + sf::Vector2i(0, 1));
		}

		if (Reproduce > 0) {
			bool Reproduced = AddAgentFromCopy(Agents[i]);
			Agents[i]->ReproductionCounts += Reproduced;
			Agents[i]->Energy -= BIRTH_COST * Reproduced;
		}
		// Second check if the agent should be dead
		if (Agents[i]->Energy < AGENT_MINIMUM_ENERGY || Agents[i]->Energy > AGENT_MAXIMUM_ENERGY + 20) AgentsToDestroy.push_back(Agents[i]);
	}

	int AgentDestroyCount = AgentsToDestroy.size();
	for (int i = 0;i < AgentDestroyCount;++i) {
		if (USE_AUTOMATIC_SELECTION && (AgentsToDestroy[i]->ReproductionCounts > BestScore)) {
			if (BestNetwork != nullptr) delete BestNetwork;
			BestNetwork = new FFNN(*AgentsToDestroy[i]->NNController);
			BestScore = AgentsToDestroy[i]->ReproductionCounts;
		}
		DestroyAgent(AgentsToDestroy[i]);
	}
	MutexRenderUpdateLoop.unlock();

}

bool Simulation::SetAgentPosition(Agent* agent, sf::Vector2i Position) {
	int x = Position.x + MapSize / 2;
	int y = Position.y + MapSize / 2;
	bool MovmentAllowed = (-1 < x && x < MapSize && -1 < y && y < MapSize ? Map[x][y].Agent == nullptr : false);
	if (MovmentAllowed) {
		Map[agent->Position.x + MapSize/2][agent->Position.y + MapSize/2].Agent = nullptr;
		agent->SetPosition(Position);
		Map[x][y].Agent = agent;
	}
	return MovmentAllowed;
}

bool Simulation::AttackAtPosition(sf::Vector2i Position) {
	int x = Position.x + MapSize / 2;
	int y = Position.y + MapSize / 2;

	if (x < 0 || x >= MapSize || y < 0 || y >= MapSize) return false;
	if (Map[x][y].Agent == nullptr) return false;

	Map[x][y].Agent->IsAttacked = true;
	Map[x][y].Agent->Energy -= COST_WHEN_ATTACKED;
	return true;
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
    agent->NNController->RandomizeByChance(INITIAL_MUTATION_CHANCE, MUTATION_APLITUDE);
	int x = agent->Position.x + MapSize / 2;
	int y = agent->Position.y + MapSize / 2;
	Map[x][y].Agent = agent;
	Agents.push_back(agent);
}

void Simulation::PenaliseForPossibleNeighbourLimit(Agent* agent) {
	sf::Vector2i pos1 = agent->Position + sf::Vector2i(-1, 0);
	sf::Vector2i pos2 = agent->Position + sf::Vector2i(1, 0);
	sf::Vector2i pos3 = agent->Position + sf::Vector2i(0, -1);
	sf::Vector2i pos4 = agent->Position + sf::Vector2i(0, 1);

	int NeighborCount = 0;

	if (IsPositionValid(pos1))  NeighborCount += Map[pos1.x][pos1.y].Agent != nullptr;
	if (IsPositionValid(pos2))  NeighborCount += Map[pos2.x][pos2.y].Agent != nullptr;
	if (IsPositionValid(pos3))  NeighborCount += Map[pos3.x][pos3.y].Agent != nullptr;
	if (IsPositionValid(pos4))  NeighborCount += Map[pos4.x][pos4.y].Agent != nullptr;

	if (NeighborCount > 2) agent->Energy -= OVER_2_NEIGHBOURS_COST * (NeighborCount - 2);
}

bool Simulation::IsPositionValid(sf::Vector2i Position) {
	return Position.x > -1 && Position.x < MapSize&& Position.y > -1 && Position.y < MapSize;
}

bool Simulation::AddAgentFromCopy(Agent* agent) {

	if (agent->Energy < BIRTH_COST) return false;

	int x = agent->Position.x + MapSize / 2;
	int y = agent->Position.y + MapSize / 2;

	bool Left = (x - 1 > -1 ? Map[x - 1][y].Agent == nullptr : false);
	bool Right = (x + 1 < MapSize ? Map[x + 1][y].Agent == nullptr : false);
	bool Up = (y - 1 > -1 ? Map[x][y - 1].Agent == nullptr : false);
	bool Down = (y + 1 < MapSize ? Map[x][y + 1].Agent == nullptr : false);

	if (!Left && !Right && !Up && !Down) return false;

	Agent* Copy = new Agent(*agent);
	
	for (;;) {
		if (Left && rand() < RAND_MAX/2) {
			Copy->SetPosition(agent->Position + sf::Vector2i(-1, 0));
			break;
		}
		else if (Right && rand() < RAND_MAX / 2) {
			Copy->SetPosition(agent->Position + sf::Vector2i(1, 0));
			break;
		}
		else if (Up && rand() < RAND_MAX / 2) {
			Copy->SetPosition(agent->Position + sf::Vector2i(0, -1));
			break;
		}
		else if (Down && rand() < RAND_MAX / 2) {
			Copy->SetPosition(agent->Position + sf::Vector2i(0, 1));
			break;
		}
	}

	if (rand() < RAND_MAX * CHANCE_OF_MUTATED_COPY) {
		Copy->NNController->RandomizeByChance(MUTATION_CHANCE_ON_COPY, MUTATION_APLITUDE);
		if (rand() < RAND_MAX * COLOR_CHANCE_CHANCE) {
			Copy->Color = sf::Color(GenerateColor());
		}
	}

	Copy->Energy = AGENT_STARTING_ENERGY;

	Map[Copy->Position.x + MapSize / 2][Copy->Position.y + MapSize / 2].Agent = Copy;
	Agents.push_back(Copy);
	return true;
}

void Simulation::DestroyAgent(Agent* agent) {
	int idx = -1;
	for (int i = 0;i < Agents.size();++i) {
		if (agent == Agents[i]) {
			idx = i;
			break;
		}
	}
	Map[agent->Position.x + MapSize / 2][agent->Position.y + MapSize / 2].Agent = nullptr;
	Agents.erase(Agents.begin() + idx);
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
	return sf::Color::Color(rand() % 200 + 100, rand() % 200 + 100, rand() % 200 + 100);
}

TileData Simulation::GetTileDataAt(int x, int y) {
	int a = x;int b = y;
	x += MapSize / 2;
	y += MapSize / 2;
	TileData tileData;
	if ((-1 < x && x < MapSize && -1 < y && y < MapSize ? Map[x][y].Agent != nullptr : false)) {
		tileData.R = Map[x][y].Agent->Color.r / 255.0f;
		tileData.G = Map[x][y].Agent->Color.g / 255.0f;
		tileData.B = Map[x][y].Agent->Color.b / 255.0f;
		tileData.Signal = Map[x][y].Agent->CurrentSignalState;
		tileData.AgentAttacks = Map[x][y].Agent->Attacks;
		tileData.AgentIsAttacked = Map[x][y].Agent->IsAttacked;
		tileData.Energy = Map[x][y].Energy;
		tileData.EnergyCapacity = Map[x][y].MaxEnergy;
	}
	else {
		tileData.R = -1;
		tileData.G = -1;
		tileData.B = -1;
		tileData.Signal = 0;
		tileData.AgentAttacks = 0;
		tileData.AgentIsAttacked = 0;
		tileData.Energy = 0;
		tileData.EnergyCapacity = 0;
	}
	return tileData;
}

float* Simulation::CompileAgentInput(Agent* agent) {

	// RGBSAA|RGBSAA|RGBSAA|...|RGBSAA|OwnSignalIn|UnderTileEnergy|Memory(30 values)|

	int index = 0;
	const int ViewRange = 4;

	static float Input[AGENT_INPUT_SIZE];

	for (int i = agent->Position.x - AGENT_VIEW_AREA;i <= agent->Position.x + AGENT_VIEW_AREA;++i) {
		for (int j = agent->Position.y - AGENT_VIEW_AREA;j <= agent->Position.y - AGENT_VIEW_AREA;++j) {
			TileData Data = GetTileDataAt(i,j);
			Input[index++] = Data.R; //R
			Input[index++] = Data.G; //G
			Input[index++] = Data.B; //B
			Input[index++] = 1 * Data.Signal + 0 * !Data.Signal; //S
			Input[index++] = 1 * Data.AgentAttacks + -1 * !Data.AgentAttacks;
			Input[index++] = 1 * Data.AgentIsAttacked + -1 * !Data.AgentIsAttacked;
		}
	}

	Input[index++] = (agent->Energy / AGENT_MAXIMUM_ENERGY) * 2 - 1;

	TileData UnderTile = GetTileDataAt(agent->Position.x, agent->Position.y);
	Input[index++] = (UnderTile.Energy / (float)MAX_ENERGY_IN_GENERATED_TILE) * 2 - 1;

	for (int i = 0;i < AGENT_MEMORY_SIZE;++i) {
		Input[index++] = agent->Memory[i];
	}

	return Input;

}