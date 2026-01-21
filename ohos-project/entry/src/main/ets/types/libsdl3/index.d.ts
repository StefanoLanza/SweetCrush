declare module 'libsdl3.so' {
  export function sdlCallbackInit (cb: object, resManager: object) : void;
  export function sdlLaunchMain (lib: string, func: string) : number;
  export function sdlKeyEvent (scancode: number, type: number) : number;
  export function sdlMouseEvent (action: number, x: number, y: number) : number;
  export function sdlTextAppend (str: string) : number;
  export function sdlTextEditing (str: string, loc: number, length: number) : number;
  export function sdlDialogExecCallback () : void;
  export function sdlDialogClearSelection () : void;
  export function sdlDialogFileSelected (path: string) : void;
  export function sdlSendDialogStatus (idx: number) : void;
  export function sdlSendLowMemoryEvent () :void;
  export function sdlSendDestroyEvent () :void;
  export function sdlOnForeground () :void;
  export function sdlOnBackground () :void;
  export function sdlDarkModeChanged (enabled: boolean) :void;
}
