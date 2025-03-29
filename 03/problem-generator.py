"""
    CLI utility to generate pddl problem for Multiagent Path Finding (MAPF)
"""

PROMPT = " >>> "

agents = {}
vertecies = set()
edges = set()

def set_to_str(s):
    return str(s) if s else "{}"

def menu():
    global agents, vertecies, edges
    while True:
        print(f"Agents:    {agents}")
        print(f"Vertecies: {set_to_str(vertecies)}")
        print(f"Edges:     {set_to_str(edges)}\n")
        print( "Would you like to: ")
        print( "A - add or remove agents")
        print( "V - add or remove vertecies")
        print( "E - add or remove edges")
        print( "S - save the problem")
        print( "Q - quit")
        try:
            response = input(PROMPT)
        except EOFError:
            print()
            break
        print(response)
        if len(response) == 0:
            print("Empty Response")
            continue
        elif len(response) > 1:
            print("Response longer than expected, only first character is taken into account")
        match response[0]:
            case 'A':
                add_or_remove_agents()
            case 'V':
                add_or_remove_vertecies()
            case 'E':
                add_or_remove_edges()
            case 'S':
                save_problem()
            case 'Q':
                    break
            case _:
                print("Unknown command")

def add_or_remove_agents():
    global agents, vertecies
    print("Commands:")
    print("  + <agent-name> <start-pos> <finish-pos> - add agent and its starting and finishing position")
    print("  - <agent-name>                          - remove agent")
    print("  ?                                       - list all agents")
    print("  q                                       - quit agents submenu")
    inmenu = True
    while inmenu:
        try:
            response = input(PROMPT)
        except EOFError:
            print()
            break
        match response[0]:
            case '+':
                [name, pos, fin] = response[1:].strip().split(' ')
                name = name.strip()
                pos = pos.strip()
                if name in agents.keys():
                    print(f'"{name}" agent is already included')
                else:
                    if pos in vertecies and fin in vertecies:
                        agents[name] = (pos, fin)
                        print(f'"{name}" agent was added')
                    else:
                        print(f"There is no vertex {pos} or {fin}")
            case '-':
                name = response[1:].strip()
                if name in agents:
                    del agents[name]
                    print(f'"{name}" vertex was removed')
                else:
                    print(f'No vertex "{name}" to remove')
            case '?':
                for agent, (pos, fin) in agents.items():
                    print('    "' + agent + '" starts at "' + pos + '" finishes at "' + fin + '"')
            case 'q':
                inmenu = False
            case _:
                print("Unknown command")

def add_or_remove_vertecies():
    global vertecies
    print("Commands:")
    print("  + <name> - add vertex")
    print("  - <name> - remove vertex")
    print("  ?        - list all vertecies")
    print("  q        - quit vertex submenu")
    inmenu = True
    while inmenu:
        try:
            response = input(PROMPT)
        except EOFError:
            print()
            break
        match response[0]:
            case '+':
                name = response[1:].strip()
                if name in vertecies:
                    print(f'"{name}" vertex is already included')
                else:
                    vertecies.add(name)
                    print(f'"{name}" vertex was added')
            case '-':
                name = response[1:].strip()
                if name in vertecies:
                    vertecies.remove(name)
                    print(f'"{name}" vertex was removed')
                else:
                    print(f'No vertex "{name}" to remove')
            case '?':
                for vertex in vertecies:
                    print('    "' + vertex + '"')
            case 'q':
                inmenu = False
            case _:
                print("Unknown command")


def add_or_remove_edges():
    global vertecies, edges
    print("Commands:")
    print("  + <name1> <name2> - add edge")
    print("  - <name1> <name2> - add edge")
    print("  ?                 - list all edges")
    print("  q                 - quit edges submenu")
    inmenu = True
    while inmenu:
        try:
            response = input(PROMPT)
        except EOFError:
            print()
            break
        match response[0]:
            case '+':
                [name1, name2] = response[1:].strip().split(' ')
                if not name1 in vertecies:
                    print(f'There is no vertex "{name1}"')
                elif not name1 in vertecies:
                    print(f'There is no vertex "{name2}"')
                elif (name1, name2) in edges or (name2, name1) in edges:
                    print(f"Edge {name1}-{name2} is already included")
                else:
                    edges.add((name1, name2))
                    print(f'Edge {name1}-{name2} was added')
            case '-':
                [name1, name2] = response[1:].strip().split(' ')
                if not name1 in vertecies:
                    print(f'There is no vertex "{name1}"')
                elif not name1 in vertecies:
                    print(f'There is no vertex "{name2}"')
                elif (name1, name2) in edges:
                    edges.remove((name1, name2))
                    print(f"Edge {name1}-{name2} is removed")
                elif (name2, name1) in edges:
                    edges.remove((name2, name1))
                    print(f"Edge {name1}-{name2} is removed")
                else:
                    print(f'No edge {name1}-{name2} to remove')
            case '?':
                for s1, s2 in edges:
                    print('    ' + s1 + '-' + s2)
            case 'q':
                inmenu = False
            case _:
                print("Unknown command")

def save_problem():
    global agents, vertecies, edges
    filename = input("Enter filename: ")
    with open(filename, 'w') as f:
        f.write("(define (problem mapf)\n")
        f.write("    (:domain graph-with-agents)\n")
        f.write("\n")
        f.write("    (:objects\n")
        f.write("        " + ', '.join(agents.keys()) + ' - agent\n')
        f.write("        " + ', '.join(vertecies) + ' - vertex\n')
        f.write("    )\n")
        f.write("\n")
        f.write("    (:init\n")
        for agent, (start, _) in agents.items():
            f.write(f"        (is-at {agent} {start})\n")
            f.write(f"        (occupied {start})\n")
        f.write("\n")
        for v, u in edges:
            f.write(f"        (edge {v} {u})\n")
            f.write(f"        (edge {u} {v})\n")

        f.write("    )\n")
        f.write("\n")
        f.write("    (:goal ( and\n")
        for agent, (_, finish) in agents.items():
            f.write(f"        (is-at {agent} {finish})\n")
        f.write("    ))\n")
        f.write(")\n")

if __name__ == "__main__":
    menu()
