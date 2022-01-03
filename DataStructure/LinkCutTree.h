// Link Cut Tree; copied from https://codeforces.com/blog/entry/75885
// - Index from 1
// - T needs to support + operation
//   For subtree queries -> requires - operation
//   --> see this comment for how to handle it: https://codeforces.com/blog/entry/67637?#comment-650424
// - Not using template here, since inheritance becomes very ugly
//
// Tested:
// - https://judge.yosupo.jp/problem/dynamic_tree_subtree_add_subtree_sum
// - https://judge.yosupo.jp/problem/dynamic_tree_vertex_set_path_composite
// - https://judge.yosupo.jp/problem/dynamic_tree_vertex_add_subtree_sum
using T = long long;
struct SplayTree { // can we replace SplayTreeById and use this only?
    struct Node {
        array<int, 2> child = {0, 0};
        int parent = 0;

        // Path aggregates
        // - path[0] = go from left -> right
        // - path[1] = go from right -> left
        array<T, 2> path;  // default to T constructor
        T self;

        // Subtree aggregates
        T sub, vir;

        // for reverse. Lazy tags can be implemented similarly (?)
        bool reverse = false;
    };
    vector<Node> nodes;

    SplayTree(int n) : nodes(n + 1) {}

    void splay(int x) {
        for (pushDown(x); ~getDirection(x); ) {
            int y = nodes[x].parent;
            int z = nodes[y].parent;
            pushDown(z);
            pushDown(y);
            pushDown(x);
            int dx = getDirection(x);
            int dy = getDirection(y);
            if (~dy) rotate(dx != dy ? x : y);
            rotate(x);
        }
    }

// private:
    // Return t where nodes[parent(x)].child[t] == x
    int getDirection(int x) {
        int p = nodes[x].parent;
        if (!p) return -1;
        return nodes[p].child[0] == x ? 0 : nodes[p].child[1] == x ? 1 : -1;
    }

    /**
     * Before:
     *    z
     *    |
     *    y
     *   /
     *  x
     *   \
     *   xchild
     * 
     * After:
     *    z
     *    |
     *    x
     *     \
     *      y
     *     /
     *   xchild
     */
    void rotate(int x) {
        int y = nodes[x].parent, dx = getDirection(x);
        int z = nodes[y].parent, dy = getDirection(y);

        setChild(y, nodes[x].child[!dx], dx);
        setChild(x, y, !dx);
        if (~dy) setChild(z, x, dy);
        nodes[x].parent = z;
    }

    void pushDown(int x) {
        if (!x) return;
        if (nodes[x].reverse) {
            auto [l, r] = nodes[x].child;
            nodes[l].reverse ^= 1;
            nodes[r].reverse ^= 1;

            swap(nodes[x].child[0], nodes[x].child[1]);
            swap(nodes[x].path[0], nodes[x].path[1]);
            nodes[x].reverse = false;
        }
    }

    void pushUp(int x) {
        auto [l, r] = nodes[x].child;
        pushDown(l); pushDown(r);

        nodes[x].path[0] = nodes[l].path[0] + nodes[x].self + nodes[r].path[0];
        nodes[x].path[1] = nodes[r].path[1] + nodes[x].self + nodes[l].path[1];

        nodes[x].sub = nodes[x].vir + nodes[l].sub + nodes[r].sub + nodes[x].self;
    }

    void setChild(int x, int y, int dir) {
        nodes[x].child[dir] = y;
        nodes[y].parent = x;
        pushUp(x);
    }
};

struct LinkCut : SplayTree {
    LinkCut(int n) : SplayTree(n) {}

    void link(int u, int v) {
        reroot(u);
        access(v);

        nodes[v].vir += nodes[u].sub;
        nodes[u].parent = v;
        pushUp(v);
    }

    void cut(int u, int v) {
        reroot(u);
        access(v);

        nodes[v].child[0] = nodes[u].parent = 0;
        pushUp(v);
    }

    T getPath(int u, int v) {
        reroot(u);
        access(v);
        return nodes[v].path[1];
    }

    void set(int u, T val) {
        access(u);
        nodes[u].self = val;
        pushUp(u);
    }

    T get(int u) {
        return nodes[u].self;
    }

// private:
    void reroot(int x) {
        access(x);
        nodes[x].reverse ^= 1;
        pushDown(x);
    }

    int access(int x) {
        int u = x, v = 0;
        for (; u; v = u, u = nodes[u].parent) {
            splay(u);
            int& ov = nodes[u].child[1];
            nodes[u].vir += nodes[ov].sub;
            // T requires subtract for subtree queries
            nodes[u].vir -= nodes[v].sub;

            ov = v; pushUp(u);
        }
        return splay(x), v;
    }
};
