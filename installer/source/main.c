// #define DEBUG_SOCKET
#define DEBUG_IP "192.168.2.2"
#define DEBUG_PORT 9023

#include <ps4.h>
#include <stdbool.h>

#include "common.h"
#include "config.h"
#include "kpayloads.h"
#include "path.h"
#include "plugins.h"
#include "version.h"
#include "patch.h"

// TODO: Where should this go? `common.c` doesn't feel right
// Apply target ID spoofing if configured
static void set_target_id(char *tid) {
  // The function input is from a controlled source and is already checked
  int hex;
  sscanf(tid, "%x", &hex);

  // Longest string for this buffer is 23 chars + 1 null term
  char buffer[0x100] = {0};
  int buffer_size = sizeof(buffer);
  switch (hex) {
  case 0:
    break;
  case 0x80:
    snprintf(buffer, buffer_size, "诊断版");
    break;
  case 0x81:
    snprintf(buffer, buffer_size, "开发机");
    break;
  case 0x82:
    snprintf(buffer, buffer_size, "测试机");
    break;
  case 0x83:
    snprintf(buffer, buffer_size, "日本");
    break;
  case 0x84:
    snprintf(buffer, buffer_size, "美国");
    break;
  case 0x85:
    snprintf(buffer, buffer_size, "欧洲");
    break;
  case 0x86:
    snprintf(buffer, buffer_size, "韩国");
    break;
  case 0x87:
    snprintf(buffer, buffer_size, "英国");
    break;
  case 0x88:
    snprintf(buffer, buffer_size, "墨西哥");
    break;
  case 0x89:
    snprintf(buffer, buffer_size, "澳大利亚和新西兰");
    break;
  case 0x8A:
    snprintf(buffer, buffer_size, "南亚");
    break;
  case 0x8B:
    snprintf(buffer, buffer_size, "台湾");
    break;
  case 0x8C:
    snprintf(buffer, buffer_size, "俄罗斯");
    break;
  case 0x8D:
    snprintf(buffer, buffer_size, "中国");
    break;
  case 0x8E:
    snprintf(buffer, buffer_size, "香港");
    break;
  case 0x8F:
    snprintf(buffer, buffer_size, "巴西");
    break;
  case 0xA0:
    snprintf(buffer, buffer_size, "Kratos");
    break;
  default:
    printf_notification("欺骗: 未知...\n检查你的 `" HEN_INI "` 文件");
    return;
  }

  if (hex > 0 && spoof_target_id(hex) != 0) {
    printf_notification("错误: 无法欺骗目标 ID");
    return;
  }
}

int _main(struct thread *td) {
  UNUSED(td);

  found_version = 0;
  initKernel();
  initLibc();

#ifdef DEBUG_SOCKET
  initNetwork();
  DEBUG_SOCK = SckConnect(DEBUG_IP, DEBUG_PORT);
#endif

  uint16_t fw_version = get_firmware();
  if (fw_version < MIN_FW || fw_version > MAX_FW) {
    printf_notification("不支持的固件版本");
    return -1;
  }

  // Jailbreak the process
  jailbreak();

  // Check temp file to prevent re-running HEN
  if (file_exists(IS_INSTALLED_PATH)) {
    printf_notification("HEN 已安装。跳过...");
    return 0;
  }

  // Apply all HEN kernel patches
  install_patches();

  // Initialize config
  struct configuration config;
  init_config(&config);

  const bool ver_match = config.config_version != DEFAULT_CONFIG_VERSION;
  const bool found_ver = found_version == 0;
  bool kill_ui = config.enable_plugins;
  const int wait_sec = 5;
  const int u_to_sec = 1000 * 1000;
  int sleep_sec = kill_ui ? wait_sec : 1;
  if (file_exists(HDD_INI_PATH) && (ver_match || found_ver)) {
    const char *reason = " 未知!";
    if (ver_match) {
      reason = " 已过期!";
    } else if (found_ver) {
      reason = " 未找到!";
    }
    printf_debug("配置版本不匹配\n");
    printf_debug("配置版本: %d\n", config.config_version);
    printf_debug("找到的版本: %d\n", found_version);
    upload_ini(HDD_INI_PATH);
    bool found_usb = file_exists(USB_INI_PATH) == 1;
    if (found_usb) {
      upload_ini(USB_INI_PATH);
    }
    printf_notification("配置版本 (%d/%d)%s\n"
                        "正在更新 %s%s 上的设置文件...", config.config_version, DEFAULT_CONFIG_VERSION, reason, "硬盘", found_usb ? " 和 USB" : "");
    init_config(&config);
    // sleep so user can see welcome message before shellui restarts
    // always sleep for `wait_sec` so other notifications aren't shown
    usleep(wait_sec * u_to_sec);
  }

  if (config.exploit_fixes) {
    printf_debug("应用漏洞修复...\n");
    exploit_fixes();
  }

  if (config.mmap_patches) {
    printf_debug("应用 mmap 补丁...\n");
    mmap_patch();
  }

  if (config.block_updates) {
    printf_debug("阻止更新...\n");
    block_updates();
  }

  if (config.disable_aslr) {
    printf_debug("禁用 ASLR...\n");
    disable_aslr();
  }

  if (config.nobd_patches) {
    printf_debug("安装 NoBD 补丁...\n");
    no_bd_patch();
  }

  // Install and run kpayload
  install_payload(&config);

  // Do this after the kpayload so if the user spoofs it doesn't affect checks in the kpayload
  if (config.target_id[0] != '\0') {
    printf_debug("设置新的 target ID...\n");
    set_target_id(config.target_id);
  }

  if (config.upload_prx) {
    printf_debug("将插件 PRX 写入磁盘...\n");
    upload_prx_to_disk();
  }

  if (!config.skip_patches) {
    InstallShellCoreCodeForAppinfo();
  }

  // Create temp file to prevent re-running HEN
  touch_file(IS_INSTALLED_PATH);
  printf_notification("欢迎使用 HEN %s", VERSION);

  const char *proc = kill_ui ? "SceShellUI" : NULL;
  if (kill_ui) {
    usleep(sleep_sec * u_to_sec);
    printf_notification("HEN 将在 %d 秒后\n重启 %s...", sleep_sec, proc);
    usleep(sleep_sec * u_to_sec);
  }

#ifdef DEBUG_SOCKET
  printf_debug("关闭 socket...\n");
  SckClose(DEBUG_SOCK);
#endif

  // this was chosen because SceShellCore will try to restart this daemon if it crashes
  // or manually killed in this case
  if (config.enable_plugins && file_exists(PRX_SERVER_PATH)) {
    kill_proc("ScePartyDaemon");
  }
  // SceShellUI is also monitored by SceShellCore
  kill_proc(proc);

  return 0;
}
