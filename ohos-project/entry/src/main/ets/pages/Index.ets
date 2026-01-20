import { hilog } from '@kit.PerformanceAnalysisKit';
import * as SDL from 'libsdl3.so';
import { intl } from '@kit.LocalizationKit';
import { promptAction } from '@kit.ArkUI';
import { inputMethod } from '@kit.IMEKit'
import { pasteboard } from '@kit.BasicServicesKit';
import { common } from '@kit.AbilityKit';
import { batteryInfo } from '@kit.BasicServicesKit';

const DOMAIN = 0x0000;

let controller = inputMethod.getController()
let context: UIContext
let targetText = "";

export class ArkNapiCallback {
  inputShown: boolean = false;
  // The application entry point, called on a dedicated thread (SDLThread)
  onMainLaunch() {
    // TODO Pass args
    SDL.sdlLaunchMain("libApp.so", "SDL_main");
  }

  terminateApp() {
    // This effectively "destroys" the app and closes the process
    let abilityContext = context.getHostContext() as common.UIAbilityContext
    abilityContext.terminateSelf((err) => {
      if (err.code) {
        console.error(`Failed to terminate app: ${err.message}`);
      }
    });
  }

  showDialog(title: string, message: string, indexMapping: number[], names: string[]) {
    let target: promptAction.Button[] = []
    for (let i = 0; i < names.length; i++)
    {
      target.push({ text: names[i], color: '#999999' })
    }
    promptAction.showDialog({ title: title, message: message, buttons: target }).then((a) => {
      SDL.sdlSendDialogStatus(indexMapping[a.index])
    })
  }
  fetchLocale(): string {
    let locale = new intl.Locale()
    return locale.language + "_" + locale.region
  }

  openLink(url: string) {
    let con = context.getHostContext() as common.UIAbilityContext
    con.openLink(url)
  }

  hasBattery(): number {
    return batteryInfo.isBatteryPresent ? 1 : 0
  }
  batteryCharging(): number {
    return batteryInfo.chargingStatus == batteryInfo.BatteryChargeState.ENABLE ? 1 : 0
  }
  batteryCharged(): number {
    return batteryInfo.chargingStatus == batteryInfo.BatteryChargeState.FULL ? 1 : 0
  }
  batteryPercent(): number {
    return batteryInfo.batterySOC
  }

  setPasteboardString(text: string) {
    let pasteData: pasteboard.PasteData = pasteboard.createData(pasteboard.MIMETYPE_TEXT_PLAIN, text);
    let systemPasteboard: pasteboard.SystemPasteboard = pasteboard.getSystemPasteboard();
    systemPasteboard.setData(pasteData, (err, data) => {
      if (err) {
        hilog.error(DOMAIN, 'testTag', 'Failed to set PasteData. Cause: ' + err.message);
        return;
      }

      hilog.info(DOMAIN, 'testTag', 'Succeed in setting PasteData.');
    });
  }

  isScreenKeyboardShown(): number {
    return this.inputShown ? 1 : 0;
  }

  hideScreenKeyboard() {
    console.info('hideScreenKeyboard.');
    try {
      controller.hideTextInput()
      this.inputShown = false
    } catch (err) {
      console.error('Failed to call hideTextInput: ' + JSON.stringify(err));
    }
    targetText = ""
  }

  showScreenKeyboard(input_type: number, x: number, y: number, w: number, h: number) {
    console.info('showScreenKeyboard.');
    try {
      controller.showTextInput()
      this.inputShown = true
    } catch (err) {
      console.error('Failed to call showTextInput: ' + JSON.stringify(err));
    }
  }

  setWindowTitle(title: string) {
    console.info('setWindowTitle.' + title);
    let abilityContext = context.getHostContext() as common.UIAbilityContext
    try {
      abilityContext.setMissionLabel(title, (err) => {
        if (err.code) {
          console.error(`Failed to set mission label: ${err.message}`);
        }
      });
    } catch (err) {
      console.error('Failed to set window title: ' + JSON.stringify(err));
    }
  }

  minimizeWindow() {
    console.info('minimizeWindow.');
  }

  async setWindowFullscreen(enable: boolean) {
    hilog.info(DOMAIN, 'testTag', 'setWindowFullscreen.');
    try {
      let abilityContext = getContext(this) as common.UIAbilityContext;
      let windowStage = abilityContext.windowStage;
      let windowClass = windowStage.getMainWindowSync();

      // 2. Hide the system bars for a true "Game Mode" fullscreen
      if (enable) {
        // Step 1: Allow content to occupy the status bar area
        await windowClass.setWindowLayoutFullScreen(true);
        //TODO await windowClass.setWindowSystemBarEnable([]); // Empty array hides all bars
      } else {
        await windowClass.setWindowLayoutFullScreen(false);
        //let names: Array<'status' | 'navigation'> = ['status', 'navigation'];
        //TODO await windowClass.setWindowSystemBarEnable(names); // Restore bars
      }
    } catch (err) {
      console.error('Failed to set fullscreen. ' + JSON.stringify(err));
    }
  }

  setWindowResizable(resizable: boolean) {
	console.info('setWindowResizable.' + resizable);
  }

  getInternalPath(): string {
    let contextt = context.getHostContext() as common.UIAbilityContext
    return contextt.filesDir
  }

  onSDLThreadComplete() {
	  console.info('SDL Thread has completed execution.');
    this.terminateApp();
  }
}

let callbackRef: ArkNapiCallback = new ArkNapiCallback()

@Entry      // tells HarmonyOS that this specific component is the Main Page (the entry point) of the current window.
@Component  // This turns a standard struct into a UI component.
struct Index {
  async aboutToAppear() {
    context = this.getUIContext()
    let abilityContext = getContext(this) as common.UIAbilityContext;
    SDL.sdlCallbackInit(callbackRef, abilityContext.resourceManager)
    focusControl.requestFocus('mainView')
    await controller.attach(false, {
      inputAttribute: {
        textInputType: inputMethod.TextInputType.TEXT,
        enterKeyType: inputMethod.EnterKeyType.DONE
      }
    });
    try {
      controller.on('insertText', (text) => {
        targetText += text;
        SDL.sdlTextAppend(text)
      })
      controller.on('deleteLeft', (i) => {
        SDL.sdlTextEditing(targetText, targetText.length - 1 - i, i);

        if (targetText.length > 0) {
          targetText = targetText.substring(0, targetText.length - i)
        }
      })
      controller.on('deleteRight', (i) => {
        SDL.sdlTextEditing(targetText, 0, i);

        if (targetText.length > 0) {
          targetText = targetText.substring(i, targetText.length)
        }
      })
    } catch (err) {
      console.error('Exception: ' + JSON.stringify(err));
    }
  }

  build() {
    Column() {
      XComponent({ id: 'mainView', type: 'surface', libraryname: 'SDL3' })
        .id('mainView')
    }
    .onKeyEvent((keyevent) => {
      SDL.sdlKeyEvent(keyevent.keyCode, keyevent.type);
    })
  }
}
