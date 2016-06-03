#include <xtask.h>

int main(void)
{
	if(systemInit() == TR_SUCCESS)
	{
		Application_Run();
	}
}

