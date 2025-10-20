#ifndef VirtualUnitConverterH
#define VirtualUnitConverterH
#include <Classes.hpp>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class VirtualUnitConverter
{
   public:
      VirtualUnitConverter(void){;}
      virtual ~VirtualUnitConverter(void){;}

   public:
	  virtual double	calcCts2User  (double dCts) = 0;
	  virtual double	calcUser2Cts  (double dUserUnit) = 0;
	  virtual bool		isSameDirection(void) = 0;
};
//---------------------------------------------------------------------------
#endif

