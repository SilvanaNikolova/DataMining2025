// #include <bits/stdc++.h>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

struct Item {
    int weight, value;
};

struct Individual {
    vector<int> chromosome;
    int fitness;
};

int M, N;
vector<Item> items;

//Mersenne Twister генератор за различно число при всяко стартиране на програмата
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

// Функция за оценка
int evaluate(const vector<int>& chrom) {
    int totalW = 0;
    int totalV = 0;

    for (int i = 0; i < N; i++) {
        if (chrom[i]) {
            totalW += items[i].weight;
            totalV += items[i].value;
        }
    }

    if (totalW > M) 
        return 0;

    return totalV;
}

// Функция за ремонт (махане на предмети, докато стане валидно)
void repair(vector<int>& chrom) {
    int totalW = 0;

    for (int i = 0; i < N; i++) {
        if (chrom[i]) 
            totalW += items[i].weight;
    }

    if (totalW <= M) 
        return;

    vector<pair<double, int>> chosen;

    for (int i = 0; i < N; i++) {
        if (chrom[i]) {
            double ratio = (double)items[i].value / items[i].weight;

            chosen.push_back({ratio, i});
        }
    }

    sort(chosen.begin(), chosen.end()); // най-лошите първи

    for (auto [ratio, idx] : chosen) {
        chrom[idx] = 0;
        totalW -= items[idx].weight;

        if (totalW <= M) 
            break;
    }
}

// Случаен индивид
Individual randomIndividual() {
    vector<int> chrom(N);
    for (int i = 0; i < N; i++) 
        chrom[i] = rng() % 2;

    repair(chrom);

    return {chrom, evaluate(chrom)};
}

// Кросоувър (one-point)
Individual crossover(const Individual& p1, const Individual& p2) {
    int point = rng() % N;
    vector<int> child(N);

    for (int i = 0; i < N; i++) {
        child[i] = (i < point ? p1.chromosome[i] : p2.chromosome[i]);
    }

    repair(child);

    return {child, evaluate(child)};
}

// Мутация
void mutate(Individual& ind, double rate = 0.01) {
    for (int i = 0; i < N; i++) {
        if ((rng() % 10000) / 10000.0 < rate) {
            ind.chromosome[i] ^= 1;
        }
    }

    repair(ind.chromosome);

    ind.fitness = evaluate(ind.chromosome);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> M >> N;
    items.resize(N);

    for (int i = 0; i < N; i++) 
        cin >> items[i].weight >> items[i].value;

    auto start = chrono::steady_clock::now();

    int POP_SIZE = 200;
    int GENERATIONS = 50;

    vector<Individual> population(POP_SIZE);

    for (int i = 0; i < POP_SIZE; i++) 
        population[i] = randomIndividual();

    int globalBest = 0;

    for (int gen = 0; gen < GENERATIONS; gen++) {
        sort(population.begin(), population.end(),
             [](const Individual& a, const Individual& b) {
                 return a.fitness > b.fitness;
             });

        globalBest = max(globalBest, population[0].fitness);

        if (gen == 0 || gen == GENERATIONS - 1 || gen % (GENERATIONS / 8) == 0) {
            cout << globalBest << "\n"; // печатаме глобалния максимум (поколения номер: 0, 6, 12, 18, 24, 30, 36, 42, 48, и 49)
        }

        vector<Individual> newPop;

        while ((int)newPop.size() < POP_SIZE) {
            //Взимам само от топ 100
            Individual p1 = population[rng() % (POP_SIZE / 2)];
            Individual p2 = population[rng() % (POP_SIZE / 2)];

            Individual child = crossover(p1, p2);

            mutate(child);
            
            newPop.push_back(child);
        }

        population = newPop;
    }

    cout << "\n" << globalBest << "\n";

    auto end = chrono::steady_clock::now();

    double elapsed = chrono::duration<double>(end - start).count();

    cerr << "Time: " << elapsed << "s\n"; // време на stderr
}