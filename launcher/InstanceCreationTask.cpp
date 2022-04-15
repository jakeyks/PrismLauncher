#include "InstanceCreationTask.h"
#include "settings/INISettingsObject.h"
#include "FileSystem.h"

//FIXME: remove this
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"

InstanceCreationTask::InstanceCreationTask(BaseVersionPtr version)
{
    m_version = version;
    m_usingLoader = false;
}

InstanceCreationTask::InstanceCreationTask(BaseVersionPtr version, QString loader, BaseVersionPtr loaderVersion)
{
    m_version = version;
    m_usingLoader = true;
    m_loader = loader;
    m_loaderVersion = loaderVersion;
}

void InstanceCreationTask::executeTask()
{
    setStatus(tr("Creating instance from version %1").arg(m_version->name()));
    {
        auto instanceSettings = std::make_shared<INISettingsObject>(FS::PathCombine(m_stagingPath, "instance.cfg"));
        instanceSettings->suspendSave();
        MinecraftInstance inst(m_globalSettings, instanceSettings, m_stagingPath);
        auto components = inst.getPackProfile();
        components->buildingFromScratch();
        components->setComponentVersion("net.minecraft", m_version->descriptor(), true);
        if(m_usingLoader)
            components->setComponentVersion(m_loader, m_loaderVersion->descriptor(), true);
        inst.setName(m_instName);
        inst.setIconKey(m_instIcon);
        instanceSettings->resumeSave();
    }
    emitSucceeded();
}
