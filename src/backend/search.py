import json, time

class searcher:
    def __init__(self):
        self.db = []
        self.load_data('addrdb.json')

        self.trienodes = []
        self.trienodes.append({})
        self.make_index()

        self.NUMBER = 10

    def load_data(self, filename):
        with open(filename, 'r') as f:
            for line in f:
                self.db.append(json.loads(line))
        print 'server: finish loading data'

    def make_index(self):
        idx = 0
        for loc in self.db:
            name = loc['name'].lower()
            node = self.trienodes[0]
            for char in name:
                if node.has_key(char):
                    node = self.trienodes[node[char]]
                else:
                    new_node_id = len(self.trienodes)
                    self.trienodes.append({})
                    node[char] = new_node_id
                    node = self.trienodes[new_node_id]
            if node.has_key('nil'):
                node['nil'].append(idx)
            else:
                node['nil'] = [idx]
            idx += 1
        print 'server: finish making index'

    def traverse(self, node):
        ret = []
        for key in node:
            if key == 'nil':
                ret.extend(node[key])
            else:
                ret.extend(self.traverse(self.trienodes[node[key]]))
        return ret

    def query(self, qstr, center_lat, center_lng):
        print 'server: query %s' % qstr

        t0 = time.clock()

        node = self.trienodes[0]
        for char in qstr.lower():
            if not node.has_key(char):
                node = None
                break
            else:
                node = self.trienodes[node[char]]

        if node is None:
            t1 = time.clock()
            return json.dumps({'time': t1-t0, 'result':[]})
        else:
            dist = []
            poi_ids = self.traverse(node)
            for poi_id in poi_ids:
                loc = self.db[poi_id]
                lat = float(loc['latlng'][0])
                lng = float(loc['latlng'][1])

                d = (lat - center_lat) ** 2 + (lng - center_lng) ** 2
                dist.append((d, poi_id))

            dist.sort()

            n_filter = self.NUMBER;
            ret = []
            for i in range(0, min(n_filter, len(dist))):
                ret.append(self.db[dist[i][1]]);

            t1 = time.clock()

            return json.dumps({'time': t1-t0, 'result':ret}, separators=(',',':'))

    def ftraverse(self, qstr):
        length = len(qstr)
        ret = []
        for i in range(length):
            for j in range(0, 26):
                s = qstr[:i] + chr(ord('a') + j) + qstr[i + 1:]
                node = self.trienodes[0]
                for char in s.lower():
                    if not node.has_key(char):
                        node = None
                        break
                    else:
                        node = self.trienodes[node[char]]
                if not(node is None):
                    ret += self.traverse(node)
        return ret

    def fquery(self, qstr, center_lat, center_lng):
        print 'server: fquery %s' % qstr

        t0 = time.clock()
        
        poi_ids = self.ftraverse(qstr)
        if len(poi_ids) > 0:
            print len(poi_ids)
            dist = []
            for poi_id in poi_ids:
                loc = self.db[poi_id]
                lat = float(loc['latlng'][0])
                lng = float(loc['latlng'][1])

                d = (lat - center_lat) ** 2 + (lng - center_lng) ** 2
                dist.append((d, poi_id))

            dist.sort()

            n_filter = self.NUMBER;
            ret = []
            for i in range(0, min(n_filter, len(dist))):
                ret.append(self.db[dist[i][1]]);

            t1 = time.clock()

            return json.dumps({'time': t1-t0, 'result':ret}, separators=(',',':'))

