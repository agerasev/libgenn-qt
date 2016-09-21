#include <genn/network.hpp>

#include <QWidget>

#include <genn/network.hpp>

#include <la/vec.hpp>

class NodeView {
public:
	vec2 pos;
	NodeView(const Node &node) {
		
	}
};

class LinkView {
public:
	vec2 pos;
};

class NetView : public QWidget {
public:
	const Network &network;
	
	
	NetView(const Network &net) : QWidget(),
		network(net)
	{
		
	}
	~NetView() = default;
};
