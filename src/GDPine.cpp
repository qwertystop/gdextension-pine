#include "GDPine.hpp"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void GDPine::_bind_methods(){
    BIND_ENUM_CONSTANT(PCSX2);
    BIND_ENUM_CONSTANT(RPCS3);
    ClassDB::bind_method(D_METHOD("init_ipc", "slot", "emulator_name", "default_slot"), &GDPine::init_ipc);
    ClassDB::bind_method(D_METHOD("init_default_ipc", "selection", "slot"), &GDPine::init_default_ipc, DEFVAL(0));
    ClassDB::bind_method(D_METHOD("Read8", "addr"), &GDPine::Read8);
    ClassDB::bind_method(D_METHOD("Read16", "addr"), &GDPine::Read16);
    ClassDB::bind_method(D_METHOD("Read32", "addr"), &GDPine::Read32);
    ClassDB::bind_method(D_METHOD("Read64", "addr"), &GDPine::Read64);
    ClassDB::bind_method(D_METHOD("ReadMany", "addr", "length_bytes", "trim_p"), &GDPine::ReadMany, DEFVAL(false));
}

godot::String GDPine::readable_ipc_status(PINE::Shared::IPCStatus status)
{
    switch (status)
    {
    case PINE::Shared::Success:
        return "Success";
        break;
    case PINE::Shared::Fail:
        return "Fail";
        break;
    case PINE::Shared::OutOfMemory:
        return "OutOfMemory";
        break;
    case PINE::Shared::NoConnection:
        return "NoConnection";
        break;
    case PINE::Shared::Unimplemented:
        return "Unimplemented";
        break;
    case PINE::Shared::Unknown:
    default:
        return "Unknown or invalid status code";
        break;
    }
}

void GDPine::init_ipc(const unsigned int slot, const godot::String emulator_name,
        const bool default_slot) noexcept try {
    ERR_FAIL_COND_MSG(pine_conn != nullptr, "IPC already initialized.");
    pine_conn = std::make_unique<PINE::Shared>(slot, emulator_name.ascii().get_data(), default_slot);
}
catch (const PINE::Shared::IPCStatus err)
{
    ERR_FAIL_MSG(vformat("IPC error %s", readable_ipc_status(err)));
}
catch (...)
{
    ERR_FAIL_MSG("Unknown PINE error.");
}

void GDPine::init_default_ipc(const DefaultConfigOption selection, unsigned int slot) noexcept try {
    switch (selection)
    {
    case PCSX2:
        init_ipc((slot == 0) ? 28011 : slot, "pcsx2", (slot == 0));
        break;
    case RPCS3:
        init_ipc((slot == 0) ? 20812 : slot, "rpcs3", (slot == 0));
        break;
    default:
        ERR_FAIL_MSG("Attempted to initialize nonexistent default.");
        break;
    }
}
catch (const PINE::Shared::IPCStatus err)
{
    ERR_FAIL_MSG(vformat("IPC error %s", readable_ipc_status(err)));
}
catch (...)
{
    ERR_FAIL_MSG("Unknown PINE error.");
}

uint8_t GDPine::Read8(uint32_t addr) noexcept try {
    ERR_FAIL_NULL_V_MSG(pine_conn, 0, "IPC not initialized.");
    return pine_conn->Read<uint8_t>(addr);
}
catch (const PINE::Shared::IPCStatus err)
{
    ERR_FAIL_V_MSG(0, vformat("IPC error %s", readable_ipc_status(err)));
}
catch (...)
{
    ERR_FAIL_V_MSG(0, "Unknown PINE error.");
}

uint16_t GDPine::Read16(uint32_t addr) noexcept try {
    ERR_FAIL_NULL_V_MSG(pine_conn, 0, "IPC not initialized.");
    return pine_conn->Read<uint16_t>(addr);
}
catch (const PINE::Shared::IPCStatus err)
{
    ERR_FAIL_V_MSG(0, vformat("IPC error %s", readable_ipc_status(err)));
}
catch (...)
{
    ERR_FAIL_V_MSG(0, "Unknown PINE error.");
}

uint32_t GDPine::Read32(uint32_t addr) noexcept try {
    ERR_FAIL_NULL_V_MSG(pine_conn, 0, "IPC not initialized.");
    return pine_conn->Read<uint32_t>(addr);
}
catch (const PINE::Shared::IPCStatus err)
{
    ERR_FAIL_V_MSG(0, vformat("IPC error %s", readable_ipc_status(err)));
}
catch (...)
{
    ERR_FAIL_V_MSG(0, "Unknown PINE error.");
}

uint64_t GDPine::Read64(uint32_t addr) noexcept try {
    ERR_FAIL_NULL_V_MSG(pine_conn, 0, "IPC not initialized.");
    return pine_conn->Read<uint64_t>(addr);
}
catch (const PINE::Shared::IPCStatus err)
{
    ERR_FAIL_V_MSG(0, vformat("IPC error %s", readable_ipc_status(err)));
}
catch (...)
{
    ERR_FAIL_V_MSG(0, "Unknown PINE error.");
}

PackedByteArray GDPine::ReadMany(uint32_t addr, uint32_t length_bytes, bool trim_p) noexcept try {
    PackedByteArray result = {};
    ERR_FAIL_COND_V_MSG(addr > (UINT32_MAX - length_bytes), result, 
                        vformat("Max extent of read (%d+%d=%d) out of range (max %d).",
                        addr, length_bytes, addr+length_bytes, UINT32_MAX));
    ERR_FAIL_NULL_V_MSG(pine_conn, result, "IPC not initialized.");
    // Make a batch of reads
    pine_conn->InitializeBatch();
    for (uint32_t i = 0; i < length_bytes; i++)
    {
        pine_conn->Read<uint8_t, true>(addr+i);
    }
    PINE::Shared::BatchCommand batch = pine_conn->FinalizeBatch();
    pine_conn->SendCommand(batch);
    // Now we need to read all of those into the result
    result.resize(length_bytes);
    uint8_t response;
    uint8_t trim_index = 0;
    for (uint32_t i = 0; i < length_bytes; i++)
    {
        response = pine_conn->GetReply<PINE::Shared::MsgRead8>(batch, i);
        result.set(i, response);
        // trim trailing zeroes during initial scan if requested
        if (trim_p && (response != 0))
        {
            trim_index = i;
        }
    }
    if (trim_p)
    {
        result.resize(trim_index + 1);
    }
    return result;
}
catch (const PINE::Shared::IPCStatus err)
{
    ERR_FAIL_V_MSG(PackedByteArray{}, vformat("IPC error %s", readable_ipc_status(err)));
}
catch (...)
{
    ERR_FAIL_V_MSG(PackedByteArray{}, "Unknown PINE error.");
}

GDPine::GDPine(){
    // Initialize any variables here.
}

GDPine::~GDPine(){
    // Add your cleanup here.
}