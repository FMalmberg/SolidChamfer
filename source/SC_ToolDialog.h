#ifndef SC_TOOLDIALOG
#define SC_TOOLDIALOG

#include "c4d.h"
#include "SolidChamfer.h"


class SC_ToolDialog: public SubDialog
{
	private:
		BaseContainer *data;
	public:
		SC_ToolDialog(BaseContainer *bc) { data = bc; }

		virtual Bool CreateLayout(void);
		virtual Bool InitValues(void);

		static LONG GetSubdivision( );
		
		static CORNER_STYLE GetCornerstyle();

		virtual Bool Command(LONG id,const BaseContainer &msg);
};

#endif