
//
  // Values exclude border of 1.5cm at each side.
    // DIN A5 landscape
    // DIN A5 portrait
    // DIN A4 landscape
    // DIN A4 portrait
    // DIN A3 landscape
    // DIN A3 portrait
    // letter landscape
    // letter portrait
static int frameX[10]= {0, 1020, 765, 1530, 1020, 2295, 1530, 1414, 1054 };
static int frameY[10]= {0, 765, 1020, 1020, 1530, 1530, 2295, 1054, 1414 };

Frame::paint?

void SchematicDoc::paintFrame(ViewPainter *p)
{ untested();
  // dimensions:  X cm / 2.54 * 144
  int xall, yall;
  if(!sizeOfFrame(xall, yall))
    return;
  p->setPen(QPen(Qt::darkGray,1));
  //p->setPen(QPen(Qt::black,0));
  int d = p->LineSpacing + int(4.0 * p->Scale);
  int x1_, y1_, x2_, y2_;
  p->map(xall, yall, x1_, y1_);
  x2_ = int(xall * p->Scale) + 1;
  y2_ = int(yall * p->Scale) + 1;
  p->drawRect(x1_, y1_, -x2_, -y2_);
  p->drawRect(x1_-d, y1_-d, 2*d-x2_, 2*d-y2_);

  int z;
  int step = xall / ((xall+127) / 255);
  for(z=step; z<=xall-step; z+=step) { untested();
    p->map(z, 0, x2_, y2_);
    p->drawLine(x2_, y2_, x2_, y2_+d);
    p->drawLine(x2_, y1_-d, x2_, y1_);
  }
  char Letter[2] = "1";
  for(z=step/2+5; z<xall; z+=step) { untested();
    p->drawText(Letter, z, 3, 0);
    p->map(z, yall+3, x2_, y2_);
    p->drawText(x2_, y2_-d, 0, 0, Qt::TextDontClip, Letter);
    Letter[0]++;
  }

  step = yall / ((yall+127) / 255);
  for(z=step; z<=yall-step; z+=step) { untested();
    p->map(0, z, x2_, y2_);
    p->drawLine(x2_, y2_, x2_+d, y2_);
    p->drawLine(x1_-d, y2_, x1_, y2_);
  }
  Letter[0] = 'A';
  for(z=step/2+5; z<yall; z+=step) { untested();
    p->drawText(Letter, 5, z, 0);
    p->map(xall+5, z, x2_, y2_);
    p->drawText(x2_-d, y2_, 0, 0, Qt::TextDontClip, Letter);
    Letter[0]++;
  }

  // draw text box with text
  p->map(xall-340, yall-3, x1_, y1_);
  p->map(xall-3,   yall-3, x2_, y2_);
  x1_ -= d;  x2_ -= d;
  y1_ -= d;  y2_ -= d;
  d = int(6.0 * p->Scale);
  z = int(200.0 * p->Scale);
  y1_ -= p->LineSpacing + d;
  p->drawLine(x1_, y1_, x2_, y1_);
  p->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, _frameText[2]);
  p->drawLine(x1_+z, y1_, x1_+z, y1_ + p->LineSpacing+d);
  p->drawText(x1_+d+z, y1_+(d>>1), 0, 0, Qt::TextDontClip, _frameText[3]);
  y1_ -= p->LineSpacing + d;
  p->drawLine(x1_, y1_, x2_, y1_);
  p->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, _frameText[1]);
  y1_ -= (_frameText[0].count('\n')+1) * p->LineSpacing + d;
  p->drawRect(x2_, y2_, x1_-x2_-1, y1_-y2_-1);
  p->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, _frameText[0]);
}

// bounding_rect?
bool SchematicDoc::sizeOfFrame(int& xall, int& yall)
{ untested();
  int i = showFrame();
  if(!i) { untested();
    // don't show? why does it not have a size?!
    return false;
  }else if(i<9) { untested();
    xall = frameX[i];
    yall = frameY[i];
    return true;
  }else{ untested();
    return false;
  }
}

