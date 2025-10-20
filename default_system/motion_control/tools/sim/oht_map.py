import pandas as pd
data_dir = "../../test/map/data/"
node_data = pd.read_csv(data_dir+'node.tsv', sep='\t', usecols=['Node ID',"X", "Y", "Z"], index_col='Node ID')
link_data = pd.read_csv(data_dir+'link.tsv', sep='\t', usecols=['Link ID', 'From Node','To Node', 'Distance'], index_col='Link ID')
station_data = pd.read_csv(data_dir+'station.tsv', sep = '\t', usecols=["StationID", "Type", "ParentNode", "NextNode", "Distance", "Port ID", "PortType", "Material"], index_col='StationID')
node_offset = node_data.mean()
node_data -= node_offset

def getNodePos(nodeid):
    return (node_data.loc[nodeid]['X'], node_data.loc[nodeid]['Y'], node_data.loc[nodeid]['Z'])

def getLink(fr, to):
    linkid = '%d_%d' % (fr, to)
    return link_data.loc[linkid]

# build graph
graph = {}
for i in range(len(link_data.index)):
    begin_node, end_node = link_data.iat[i,0], link_data.iat[i,1]
    if begin_node in graph :
        graph[begin_node].append(end_node)
    else:
        graph[begin_node] = [end_node]

def calc_path_cost(path):
    return len(path)


def find_shortest_path(start, end, path=[]):
    global graph
    path.append(start)
    while True:
        if start == end:
            return path
        if not start in graph:
            return None
        if len(graph[start]) != 1 : break
        # 단일 경로 iteration
        start = graph[start][0]
        path.append(start)

    shortest = None
    for node in graph[start]:
        if node not in path:
            newpath = find_shortest_path(node, end, path[:])
            if newpath:
                if not shortest or calc_path_cost(newpath) < calc_path_cost(shortest):
                    shortest = newpath
    return shortest

if __name__ == "__main__":
    print(find_shortest_path(101, 121))
