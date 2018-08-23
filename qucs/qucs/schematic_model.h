// TODO: refactor here
class WireList : public Q3PtrList<Wire> {
};
// TODO: refactor here
class NodeList : public Q3PtrList<Node> {
};
// TODO: refactor here
class DiagramList : public Q3PtrList<Diagram> {
};
// TODO: refactor here
class ComponentList : public Q3PtrList<Component> {
	// void first(){} // GOAL: hide, still compile.
};
// TODO: refactor here
class PaintingList : public Q3PtrList<Painting> {
public:
	bool load(QTextStream *stream);
public:
	void sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax) const;
};

class SchematicModel{
private:
  SchematicModel(){}
public:
  SchematicModel(Schematic* s);
public: // stuff saved from Schematic
  QString createClipboardFile();
  void sizeOfAll(int&, int&, int&, int&, float) const;
  void simpleInsertComponent(Component* c);
  void simpleInsertWire(Wire*);
public:
  void parse(QTextStream& stream);
  bool loadDocument(QFile& /*BUG*/ file);
  bool loadPaintings(QTextStream*, PaintingList*);
  bool loadProperties(QTextStream*);
  bool loadComponents(QTextStream*);
  bool loadDiagrams(QTextStream*);
  bool loadWires(QTextStream*);

  void clear();
public:
  void merge(SchematicModel&);

public: // scene interaction
  void toScene(QGraphicsScene& s, QList<ElementGraphics*>* l=nullptr) const;
public: // obsolete.
  static void saveComponent(QTextStream& s, Component /* FIXME const */* c);
private: // TODO: actually store here.
  WireList& wires();
  NodeList& nodes();
  DiagramList& diagrams();
  PaintingList& paintings();
  ComponentList& components();
  PaintingList& symbolPaintings();
public:
  WireList const& wires() const;
  NodeList const& nodes() const;
  DiagramList const& diagrams() const;
  PaintingList const& paintings() const;
  ComponentList const& components() const;

  Schematic* doc();
private: // TODO: remove.
  Schematic* _doc;
private:
  ComponentList Components;
  NodeList Nodes;
  DiagramList Diagrams;
public: // for now.
  friend class Schematic;
};
