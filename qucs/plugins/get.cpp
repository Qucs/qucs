


namespace{

class Get : public Command {
	void do_it(istream_t&, SchematicModel*) override;
}c;

#if 0
void Get::load(istream_t& s, Object* c) const
{
// TODO: move stuff here that does not belong to leg_sch.
	auto l=doclang_dispatcher["leg_sch"];
	assert(l);
	auto L=dynamic_cast<SchematicLanguage const*>(l);
	assert(L);

	if(auto cc=dynamic_cast<SchematicSymbol*>(c)){
		while(!s.atEnd()){
			L->parse(s, cc); // BUG BUG only pass SchematicModel
			assert(s.atEnd()); // happens with legacy lang
		}
	}else if(auto cc=dynamic_cast<SubcktBase*>(c)){
		while(!s.atEnd()){
			L->parse(s, cc); // BUG BUG only pass SchematicModel
			assert(s.atEnd()); // happens with legacy lang
		}
	}else{
		unreachable();
	}

}
#endif

// TODO: any language.
void Get::do_it(istream_t& stream, SchematicModel* sckt) const
{
	assert(!implicit_hack.size());
	QString Line;
	assert(sckt);

	char mode='?';
	while(!stream.atEnd()) {
		Line = QString::fromStdString(stream.read_line());
		Line = Line.trimmed();
		if(Line.size()<2){
		}else if(Line.at(0) == '<'
				&& Line.at(1) == '/'){
			qDebug() << "endtag?" << Line;
			mode = 0;
		}else if(Line.isEmpty()){ untested();
		}else if(stream.umatch("<Components>")) { untested();
			mode='C';
			new__instance(stream, owner, sckt);
		}else if(Line == "<Symbol>") {
			mode='S';
			trace1("symbol??", stream.fullstring());
			new__instance(stream, owner, sckt);
			trace1("done symbol", stream.fullstring());
		}else if(Line == "<Wires>") {
			mode='W';
		}else if(Line == "<Diagrams>") {
			mode='D';
		}else if(Line == "<Properties>") {
			mode='Q';
			trace1("model", stream.fullString());
			new__instance(stream, owner, sckt);
		}else if(Line == "<Paintings>") {
			trace1("paintings?", stream.fullString());
			mode='P';
		}else if(Line == "<Model>") {
			mode='M';
			trace1("model", stream.fullString());
			new__instance(stream, owner, sckt);
		}else if(Line == "<Description>") {
			mode='X';
		}else{

			/// \todo enable user to load partial schematic, skip unknown components
			Element*c = nullptr;
			if(mode=='C'){
				// new__instance(stream, &owner, owner.subckt());
				c = getComponentFromName(Line);
				if(c){
					c->setOwner(owner); // owner->subckt()?
				}else{
				}
				if(Symbol* sym=dynamic_cast<Symbol*>(c) ){
					//always do this?

//					assert(s.scope());
					// what are those?! FIXME: do later.
					Symbol const* cs = sym;
					assert(cs->owner());
					sym->recreate(); // BUG: re? create symbol gfx and random other things. needs owner
					                 //  used in legacy/subcircuit.cpp
					sym->build(); // what's this?!
				}else{
				}
			}else if(mode=='S'){ untested();
				std::string typeName = findType(stream);
				trace2("modeS findtype", typeName, _lib_mod);
				// incomplete();
				try{
					if(_lib_mod || !dynamic_cast<SchematicSymbol*>(owner)){ untested();
						// TODO: always do this
						new__instance(stream, owner, sckt);
						// assert(0);
					}else{
						// incomplete. hack use parseItem.
						auto Line = QString::fromStdString(stream.fullString());

						auto ss = dynamic_cast<SchematicSymbol*>(owner);
						assert(ss);
						PaintingListLoad(Line, ss->symbolPaintings());
						trace1("symbolpaint", Line);
					}
					c = nullptr;
				}catch(...){ untested();
					incomplete();
				}
			}else if(mode=='W'){
				std::string typeName = findType(stream);
				assert(typeName=="Wire");
				assert(owner->subckt());
				auto main = owner->subckt()->find_("main");
				assert(main != owner->subckt()->end());
				assert(*main);
				auto mm=dynamic_cast<SubcktBase*>(*main);
				assert(mm);

				new__instance(stream, mm, mm->subckt());
			}else if(mode=='D'){
				new__instance(stream, owner, sckt);
			}else if(mode=='Q'){
			}else if(mode=='M'){
				// assert(false);
				// incomplete();
			}else if(mode=='X'){
				trace1("legacy_lang description", Line);
			}else{
				trace2("LSL::parse", mode, Line);
				new__instance(stream, owner, sckt);
			}

			if(c){
				trace2("pushing back", c->label(), typeid(*c).name());
				Element const* cc = c;
				assert(cc->owner() == owner);


				if(auto sym=dynamic_cast<Symbol*>(c)){
					sym->build(); // here?!
				}else{
				}


				sckt->pushBack(c);
			}else{
			}
		}
	}

	/// some components are implicit, collect them here
	while(implicit_hack.size()){ untested();
		auto cc = implicit_hack.front();
		cc->setOwner(owner);
		implicit_hack.pop_front();
		// c->precalc(); // here?!
		sckt->pushBack(cc);
	}
}
}
