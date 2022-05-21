
#ifndef BASIC_OBJECT_H_
#define BASIC_OBJECT_H_

#include "CommonFunc.h"

class BaseObject
{
public:
  BaseObject();
  ~BaseObject();
  virtual bool LoadImageFile(std::string path, SDL_Renderer* screen);
  void Render(SDL_Renderer* des, const SDL_Rect* clip = NULL);
  
  void Free();
  void SetRect(const int& x, const int& y) {rect_.x = x, rect_.y = y;};
  SDL_Rect GetRect() const {return rect_;}
  SDL_Texture* GetObject() {return pObject_;}
protected:
  SDL_Texture* pObject_;
  SDL_Rect rect_;
};


#endif