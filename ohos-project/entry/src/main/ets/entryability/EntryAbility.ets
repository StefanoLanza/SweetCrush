import { abilityAccessCtrl, AbilityConstant,
  common,
  ConfigurationConstant,
  Permissions,
  UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { promptAction, window } from '@kit.ArkUI';
import { KeyboardAvoidMode } from '@ohos.arkui.UIContext';
import { BusinessError } from '@kit.BasicServicesKit';
import * as SDL from 'libsdl3.so';

const DOMAIN = 0x0000;

const permissions: Array<Permissions> = ['ohos.permission.INPUT_KEYBOARD_CONTROLLER', 'ohos.permission.READ_PASTEBOARD'];
function reqPermissionsFromUser(permissions: Array<Permissions>, context: common.UIAbilityContext): void {
  let atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
  atManager.requestPermissionsFromUser(context, permissions).then((data) => {
    let grantStatus: Array<number> = data.authResults;
    let length: number = grantStatus.length;
    for (let i = 0; i < length; i++) {
      if (grantStatus[i] === 0) {
        // promptAction.showDialog({title: "permissions", message: "granted", buttons: [{text: 'Ok', color: '#999999'}]});
      } else {
        // promptAction.showDialog({title: "permissions", message: "not granted", buttons: [{text: 'Ok', color: '#999999'}]});
        return;
      }
    }
    // 授权成功
  }).catch((err: BusinessError) => {
    console.error(`Failed to request permissions from user. Code is ${err.code}, message is ${err.message}`);
  })
}

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
    // TODO Inform SDL
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onCreate');
  }

  onDestroy(): void {
    SDL.sdlSendDestroyEvent();
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  async onWindowStageCreate(windowStage: window.WindowStage): Promise<void> {
    // Main window is created, set main page for this ability
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    windowStage.loadContent('pages/Index', (err) => {
      reqPermissionsFromUser(permissions, this.context);

      windowStage.getMainWindow().then(async (win) => {
        try {
          win.setWindowTitleButtonVisible(false, false, false)
          win.getUIContext().setKeyboardAvoidMode(KeyboardAvoidMode.OFFSET)
          // win.setWindowLayoutFullScreen(true)
          // win.setPreferredOrientation(window.Orientation.FOLLOW_DESKTOP)
          win.setResizeByDragEnabled(false)
        } catch (error) {
          let ll: BusinessError = error;
          hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(ll));
        }
      })

      if (err.code) {
        hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err));
        return;
      }
      hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content.');
    });
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    // Ability has brought to foreground
    SDL.sdlOnForeground();
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    // Ability has back to background
    SDL.sdlOnBackground();
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onBackground');
  }

  onMemoryLevel(level: AbilityConstant.MemoryLevel) {
    // level 0: MEMORY_LEVEL_LOW (System is low on memory)
    // level 1: MEMORY_LEVEL_MEDIUM (System is very low, start purging)
    // level 2: MEMORY_LEVEL_CRITICAL (System is about to kill processes)
    console.warn(`Memory level warning: ${level}`);
    if (level == AbilityConstant.MemoryLevel.MEMORY_LEVEL_LOW)
    {
      SDL.sdlSendLowMemoryEvent();
    }
  }
}
