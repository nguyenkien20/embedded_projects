// HSE MEASUREMENT
#include <stdint.h>

#define RCC_BASE_ADDR              0x40023800UL

#define RCC_CLK_CTRL_REG (RCC_BASE_ADDR + 0x00UL)

#define RCC_CFGR_REG_OFFSET        0x08UL

#define RCC_CFGR_REG_ADDR          (RCC_BASE_ADDR + RCC_CFGR_REG_OFFSET )

#define GPIOC_BASE_ADDR            0x40020800UL


int main(){
	uint32_t *pRccCtrlReg = (uint32_t *) RCC_CLK_CTRL_REG;
	// 1. set HSEBYP
	*pRccCtrlReg |= (1<<18);
	// 2. set HSEON
	*pRccCtrlReg |= (1<<16);
	// 3. check if HSE oscillator is okay or not
	while(!(*pRccCtrlReg & (1<<17)));
	// configuration for PC9 -> output signal HSE
	uint32_t *pRccAHB1Reg = (uint32_t *) (RCC_BASE_ADDR + 0x30UL);
	*pRccAHB1Reg |= (1 << 2);

	uint32_t *pGPIOCModeReg = (uint32_t*)(GPIOC_BASE_ADDR + 0x00);

	*pGPIOCModeReg &= ~(0x3 << 18); //clear
	*pGPIOCModeReg |= ( 0x2 << 18);  //set

	uint32_t *pGPIOCAltFunHighReg = (uint32_t*)(GPIOC_BASE_ADDR + 0x24);
	*pGPIOCAltFunHighReg &= ~( 0xf << 4);

	uint32_t *pRccCfgrReg = (uint32_t *) RCC_CFGR_REG_ADDR;
	*pRccCfgrReg &= ~(3<<30);
	*pRccCfgrReg |= (1<<31);

	*pRccCfgrReg &= ~(7<<27);

	*pRccCfgrReg |= (3<<28);

	return 0;
}
