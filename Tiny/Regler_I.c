#include "Regler_I.h"


unsigned int stellCur = 0;
signed int diff;

unsigned int Regulate_I(unsigned int ist, unsigned int soll)
{
static unsigned int loopCntr = 0;

	if(soll == 0)
	{
		stellCur = 0;
		return stellCur;
	}

	if(loopCntr >= 10)
	{
		diff = ist - soll;
		loopCntr = 0;
		if(diff > 0)
		{
		/*
			if(diff > 20) 
			{
				if(stellCur > 19) stellCur -= 20;
			}
			else if(diff > 10) 
			{
				if(stellCur > 9) stellCur -= 10;
			}
			else */
			if(stellCur > 0)stellCur--;
		}
		if(diff < 0)
		{
		/*
			if(diff < 20) 
			{
				if(stellCur < 406) stellCur += 20;
			}
			else if(diff < 9) 
			{
				if(stellCur > 416) stellCur += 10;
			}
			else */
			if(stellCur < 426)stellCur++;
		}
		loopCntr = 0;
	}
	else
	{
		loopCntr++;
	}

return stellCur;
}

void ResetRegler_I()
{
	stellCur = 0;
}
