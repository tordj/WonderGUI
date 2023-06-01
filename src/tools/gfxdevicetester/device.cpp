
#include "device.h"


GfxDevice_p Device::beginRender() const
{
    m_pDevice->beginRender();
    m_pDevice->beginCanvasUpdate(m_pCanvas);
    return m_pDevice;
}


void Device::endRender() const
{
    m_pDevice->endCanvasUpdate();
    m_pDevice->endRender();
}

