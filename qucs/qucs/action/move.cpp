
// wrong base class.
class MoveSelection : public QUndoCommand {
public:
	template<class IT>
	MoveSelection(QPoint delta, SchematicDoc& ctx, IT selection)
	: _delta(delta), _ctx(ctx) { itested();
		trace1("MoveSelection", delta);
		size_t k = 0;
		for(auto i : selection){ untested();
			if(auto eg=dynamic_cast<ElementGraphics*>(i)){ untested();
				++k;
				_gfx.push_back(eg);
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}
		setText("move " + QString::number(k) + " items by "
				+ QString::number(delta.x()) + ", "
				+ QString::number(delta.y())) ;
	}
	void undo() override { untested();
		QUndoCommand::undo(); // incomplete
		do_it();
	}
	void redo() override { untested();
		QUndoCommand::redo(); // incomplete
		do_it();
	}
private:
	void do_it() { untested();
		trace2("moveSelection", _delta.x(), _delta.y());
		for(auto& d : _gfx){ untested();
			d->moveElement(_delta);
		}

		_delta = -_delta;
	}
private:
	QPoint _delta;
	SchematicDoc& _ctx; // needed?
	std::vector<ElementGraphics*> _gfx;
}; // MoveSelection
