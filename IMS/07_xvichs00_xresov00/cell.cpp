#include <iostream>

class Cell {
	public:
		bool isCar;
		bool isIntersection;
		int speed;
		int speedRewrite;
		bool xFree;
		bool isXCell;
		bool isYCell;
		bool xDirection;
		bool yDirection;
		bool xDirectionRewrite;
		bool yDirectionRewrite;
		int cycle;
		bool rewrited;
		Cell();
		void setYCell(void);
		void setXCell(void);
		void setCar(int speed, bool xDirection);
		void setIntersection(int offset);
		void switchLight();
};

Cell::Cell() {
	this->isCar = false;
	this->isIntersection = false;
	this->rewrited = false;
	this->speed = 0;
	this->xFree = false;
	this->isXCell = false;
	this->isYCell = false;
	this->cycle = 0;
	this->xDirection = false;
	this->yDirection = false;
	this->xDirectionRewrite = false;
	this->yDirectionRewrite = false;
}

void Cell::setYCell(void) {
	this->isXCell = false;
	this->isYCell = true;
};

void Cell::setXCell(void) {
	this->isXCell = true;
	this->isYCell = false;
};

void Cell::setCar(int speed, bool xDir) {
	this->isCar = true;
	this->speed = speed;
	this->xDirection =  xDir;
	this->yDirection = !xDir;
};

void Cell::setIntersection(int offset) {
	this->isIntersection = true;
	this->xFree = true;
	this->cycle = offset;
};

void Cell::switchLight() {
	if(this->isIntersection) {
		this->xFree = !this->xFree;
	}
};


