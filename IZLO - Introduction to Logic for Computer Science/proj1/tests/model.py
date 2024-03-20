from collections import Counter

STATUS_SAT = "SAT"
STATUS_UNSAT = "UNSAT"


class ModelError(Exception):
    pass

class Edge:
    def __init__(self, src, dst):
        self.src = src
        self.dst = dst

    def __eq__(self, other):
        return self.src == other.src and self.dst == other.dst
    
    def __hash__(self):
        return hash((self.src, self.dst))

    def __repr__(self):
        return f"{self.src} -> {self.dst}"

class Input:
    def __init__(self, num_of_nodes, edges):
        self.num_of_nodes = num_of_nodes
        self.edges = edges

    @staticmethod
    def load(path):
        with open(path) as f:
            lines = f.read().split("\n")
            header = lines[0].split(" ")
            num_of_nodes = int(header[0])

            edges = []
            for line in lines[1:]:
                if line not in ["", "\n"]:
                    edge = line.split(" ")
                    src = int(edge[0])
                    dst = int(edge[1])
                    edges.append(Edge(src, dst))

            return Input(num_of_nodes, edges)

    @property
    def num_of_edges(self):
        return len(self.edges)

    def compute_var_index(self, step, src, dst):
        return step * (self.num_of_nodes ** 2) + src * self.num_of_nodes + dst + 1


class Model:
    def __init__(self, status, literals, input):
        self.status = status
        self.literals = literals
        self.input = input

    def is_sat(self):
        return self.status == STATUS_SAT

    @staticmethod
    def load(path, input):
        """
        The output of the minisat always has the form:
            STATUS
            [MODEL 0]
        """
        with open(path, "r") as f:
            lines = f.read().split("\n")
            status = lines[0]

            if status == STATUS_UNSAT:
                return Model(status, None, input)
            else:
                model = lines[1].split(" ")[0:-1]  # Discard '0'

                if model == [""]:
                    return Model(status, [], input)

                model = list(map(lambda x: int(x), model))
                return Model(status, model, input)

    def __getitem__(self, key):
        var = self.input.compute_var_index(*key)

        if var in self.literals:
            return True
        elif -var in self.literals:
            return False
        else:
            return True  # variable is undefined

    def edges_in_step(self, step):
        # Return all edges taken in the given step
        acc = []
        for src in range(self.input.num_of_nodes):
            for dst in range(self.input.num_of_nodes):
                if self[step, src, dst]:
                    acc.append(Edge(src, dst))
        return acc

    def all_edges(self):
        # Return all edges taken
        acc = []
        for step in range(self.input.num_of_edges):
            acc += self.edges_in_step(step)
        return acc

    def print(self):
        print("Status:", self.status)

        if self.status == STATUS_UNSAT:
            return

        print("Model:")
        for step in range(self.input.num_of_edges):
            edges = ", ".join([str(e) for e in self.edges_in_step(step)])
            print(f"    krok {step}: {edges}")

    def check_one_valid_edge_in_step(self):
        for step in range(self.input.num_of_edges):
            edges = self.edges_in_step(step)
            if len(edges) == 0: 
                raise ModelError(f"Nesprávný model: žádná ulice v kroku {step}")
            if len(edges) > 1:
                raise ModelError(f"Nesprávný model: více ulic v kroku {step}")
            for edge in edges:
                if edge not in self.input.edges:
                    raise ModelError(f"Nesprávný model: neexistující ulice {edge} v kroku {step}")

    def check_path(self):
        edges = self.all_edges()
        duplicates = [str(edge) for (edge, count) in Counter(edges).items() if count > 1]
        if len(duplicates) > 0:
            duplicates = ", ".join(duplicates)
            raise ModelError(f"Nesprávný model: některé ulice byly navštíveny vícekrát: {duplicates}.")

        # This should not happen
        if len(edges) != len(self.input.edges):
            raise ModelError(f"Nesprávný model: některá z ulic nebyla navštívena.")

    def check_connectivity(self):
        for step in range(self.input.num_of_edges - 1):
            edges = self.edges_in_step(step)
            edges_next = self.edges_in_step(step+1)

            assert(len(edges) == 1)
            assert(len(edges_next) == 1)
            current_e = edges[0]
            next_e = edges_next[0]

            if current_e.dst != next_e.src:
                raise ModelError(f"Nesprávný model: ulice {next_e} v kroku {step+1} nenavazuje na ulici {current_e} z kroku {step}.")

    def check(self):
        self.check_one_valid_edge_in_step()
        self.check_path()
        self.check_connectivity()
