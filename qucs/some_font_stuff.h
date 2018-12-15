#ifndef QUCS_SOME_FONT_STUFF
#define QUCS_SOME_FONT_STUFF

#include <QFontMetrics> // BUG
#include <QRect>

// need some cleanup. but at least it works...

inline int get_some_width(QString const& stmp)
{
	incomplete(); // crashes.
	return 17;

  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10); 
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0);
  return smallmetrics.width(stmp); // compute width to right-align
}

class FontMetrics{
public:
	QSize size(int, QString const&) const{
		//		incomplete
		return QSize(1, 5);
		//		f.size(i,s) //sth
	}
	int height() const{
		//		incomplete
		return 0;
		QFontMetrics f(QucsSettings.font, 0); // crashes.
		return f.height();
	}
	int width(QString const& s) const{
		return 0;
		QFontMetrics f(QucsSettings.font, 0); // crashes.
		return f.width(s);
	}
	int ascent() const{
		return 1;
	}
	int descent() const{
		return 1;
	}
	int	lineSpacing()const{
		return 2;
	}
	QRect boundingRect(QString const&) const{
		return QRect(0,0,1,1);
	}
	void setSmall(){
		incomplete();
	}
};

#endif
