# Blue-Hand-Command — 性能配置（对齐 Tiny Trouble VR）

> 本文档完全参照 `E:\Tiny Trouble VR\TinyTrouble\Config\` 中的配置，Tiny Trouble 已经是跑通的 Quest VR 项目。

---

## 一、DefaultEngine.ini — RendererSettings

以下配置直接照搬 Tiny Trouble 已验证的设定，只改了项目名称相关的内容。

```ini
[/Script/Engine.RendererSettings]

; === 移动端渲染管线 ===
r.Mobile.ShadingPath=0                           ; Mobile Forward Shading
r.Mobile.AllowDeferredShadingOpenGL=False
r.Mobile.SupportGPUScene=False
r.Mobile.FloatPrecisionMode=0                    ; Half precision
r.Mobile.AntiAliasing=1                          ; MSAA
r.Mobile.AllowDitheredLODTransition=False
r.Mobile.VirtualTextures=False
r.MobileHDR=False
r.Mobile.UseHWsRGBEncoding=True
r.Mobile.Forward.EnableLocalLights=1
r.Mobile.Forward.EnableClusteredReflections=False
r.Mobile.EnableStaticAndCSMShadowReceivers=True
r.Mobile.EnableMovableLightCSMShaderCulling=True
r.Mobile.EnableNoPrecomputedLightingCSMShader=True
r.Mobile.AllowDistanceFieldShadows=True
r.Mobile.AllowMovableDirectionalLights=True
r.Mobile.EnableMovableSpotlightsShadow=False
r.Mobile.AmbientOcclusion=False
r.Mobile.DBuffer=False
r.Mobile.PlanarReflectionMode=0
r.Mobile.SupportsGen4TAA=True

; === Lumen（关闭）===
r.DynamicGlobalIlluminationMethod=0
r.ReflectionMethod=0
r.ReflectionCaptureResolution=128
r.ReflectionEnvironmentLightmapMixBasedOnRoughness=True
r.Lumen.HardwareRayTracing=False
r.Lumen.HardwareRayTracing.LightingMode=0
r.Lumen.TranslucencyReflections.FrontLayer.EnableForProject=False
r.Lumen.TraceMeshSDFs=0
r.Lumen.Reflections.HardwareRayTracing.Translucent.Refraction.EnableForProject=True

; === Nanite（关闭）===
r.Nanite.ProjectEnabled=False

; === 阴影 ===
r.Shadow.Virtual.Enable=0
r.Shadow.UnbuiltPreviewInGame=True
r.Shadow.CSMCaching=False
r.Shadow.TranslucentPerObject.ProjectEnabled=False
r.MinScreenRadiusForLights=0.030000
r.MinScreenRadiusForDepthPrepass=0.030000
r.MinScreenRadiusForCSMDepth=0.010000

; === 光追（关闭）===
r.RayTracing=False
r.RayTracing.Shadows=False
r.RayTracing.UseTextureLod=False
r.PathTracing=False

; === Substrate（关闭）===
r.Substrate=False
r.Substrate.OpaqueMaterialRoughRefraction=False
r.Substrate.Debug.AdvancedVisualizationShaders=False

; === 体积效果 ===
r.HeterogeneousVolumes=False
r.HeterogeneousVolumes.Shadows=False
r.Translucency.HeterogeneousVolumes=False
r.VolumetricFog.LightFunction=True
r.LocalFogVolume.ApplyOnTranslucent=False
r.SupportSkyAtmosphere=True
r.SupportSkyAtmosphereAffectsHeightFog=True
r.SupportLocalFogVolumes=True

; === 抗锯齿 ===
r.MSAACount=4
r.AntiAliasingMethod=0
r.TemporalAA.Upsampling=True

; === 后处理 ===
r.DefaultFeature.Bloom=False                     ; 我们关闭 Bloom
r.DefaultFeature.AmbientOcclusion=False
r.DefaultFeature.AmbientOcclusionStaticFraction=True
r.DefaultFeature.AutoExposure=False
r.DefaultFeature.AutoExposure.Method=0
r.DefaultFeature.AutoExposure.Bias=1.000000
r.DefaultFeature.AutoExposure.ExtendDefaultLuminanceRange=True
r.DefaultFeature.LocalExposure.HighlightContrastScale=0.800000
r.DefaultFeature.LocalExposure.ShadowContrastScale=0.800000
r.DefaultFeature.MotionBlur=False
r.DefaultFeature.LensFlare=False

; === 遮挡剔除（Tiny Trouble 开了）===
r.AllowOcclusionQueries=True
r.PrecomputedVisibilityWarning=False

; === 纹理 ===
r.TextureStreaming=True
r.MeshStreaming=False
r.DiscardUnusedQuality=False
Compat.UseDXT5NormalMaps=False
r.VirtualTextures=False
r.VT.EnableAutoImport=True
r.VirtualTexturedLightmaps=False
r.VT.AnisotropicFiltering=False
bEnableVirtualTextureOpacityMask=False
r.VT.TileSize=128
r.VT.TileBorderSize=4
r.vt.FeedbackFactor=16

; === VR 专用 ===
vr.InstancedStereo=False
vr.MobileMultiView=True
vr.RoundRobinOcclusion=False
xr.VRS.FoveationLevel=0
xr.VRS.DynamicFoveation=False

; === 其他 ===
r.ClearCoatNormal=False
r.NormalMapsForStaticLighting=False
r.ForwardShading=False
r.VertexFoggingForOpaque=True
r.SeparateTranslucency=False
r.TranslucentSortPolicy=0
TranslucentSortAxis=(X=0.000000,Y=-1.000000,Z=0.000000)
r.CustomDepth=1
r.CustomDepthTemporalAAJitter=True
r.PostProcessing.PropagateAlpha=0
r.DefaultFeature.LightUnits=1
r.DefaultBackBufferPixelFormat=4
r.ScreenPercentage.Default=100.000000
r.StencilForLODDither=False
r.EarlyZPass=3
r.EarlyZPassOnlyMaterialMasking=False
r.DBuffer=True
r.ClearSceneMethod=1
r.VelocityOutputPass=0
r.Velocity.EnableVertexDeformation=2
r.SelectiveBasePassOutputs=False
bDefaultParticleCutouts=False
fx.GPUSimulationTextureSizeX=1024
fx.GPUSimulationTextureSizeY=1024
r.AllowGlobalClipPlane=False
r.GBufferFormat=1
r.MorphTarget.Mode=True
r.MorphTarget.MaxBlendWeight=5.000000
r.GenerateMeshDistanceFields=False
r.DistanceFields.DefaultVoxelDensity=0.200000
r.WireframeCullThreshold=5.000000
r.AllowStaticLighting=True
r.SupportStationarySkylight=True
r.SupportLowQualityLightmaps=True
r.SupportPointLightWholeSceneShadows=True
r.Water.SingleLayerWater.SupportCloudShadow=False
r.Material.RoughDiffuse=False
r.Material.EnergyConservation=False
r.OIT.SortedPixels=False
r.HairStrands.LODMode=True
r.SkinCache.CompileShaders=False
r.SkinCache.SkipCompilingGPUSkinVF=False
r.SkinCache.DefaultBehavior=1
r.SkinCache.SceneMemoryLimitInMB=128.000000
r.GPUSkin.Support16BitBoneIndex=False
r.GPUSkin.Limit2BoneInfluences=False
r.SupportDepthOnlyIndexBuffers=True
r.SupportReversedIndexBuffers=True
r.GPUSkin.UnlimitedBoneInfluences=False
r.GPUSkin.AlwaysUseDeformerForUnlimitedBoneInfluences=False
r.GPUSkin.UnlimitedBoneInfluencesThreshold=8
DefaultBoneInfluenceLimit=(Default=0,PerPlatform=())
MaxSkinBones=(Default=65536,PerPlatform=(("Mobile", 256)))
bStreamSkeletalMeshLODs=(Default=False,PerPlatform=())
bDiscardSkeletalMeshOptionalLODs=(Default=False,PerPlatform=())
r.GPUCrashDebugging=False
r.SupportCloudShadowOnForwardLitTranslucent=False
r.LightFunctionAtlas.Format=0
r.Deferred.UsesLightFunctionAtlas=False
r.SingleLayerWater.UsesLightFunctionAtlas=False
r.Translucent.UsesLightFunctionAtlas=False
r.Translucent.UsesIESProfiles=False
r.Translucent.UsesRectLights=False
WorkingColorSpaceChoice=sRGB
RedChromaticityCoordinate=(X=0.640000,Y=0.330000)
GreenChromaticityCoordinate=(X=0.300000,Y=0.600000)
BlueChromaticityCoordinate=(X=0.150000,Y=0.060000)
```

---

## 二、SystemSettings（运行时覆盖）

```ini
[SystemSettings]
r.Nanite=0
```

---

## 三、Oculus XR 运行设置

```ini
[/Script/OculusXRHMD.OculusXRHMDRuntimeSettings]
SystemSplashBackground=Black
bAutoEnabled=False                               ; Tiny Trouble 是 False（手动启用）
XrApi=OVRPluginOpenXR
ColorSpace=P3
ControllerPoseAlignment=Default
bDynamicResolution=False
PixelDensityMin=0.800000
PixelDensityMax=1.200000
+SupportedDevices=Quest3
+SupportedDevices=QuestPro
+SupportedDevices=Quest2
SuggestedCpuPerfLevel=SustainedLow
SuggestedGpuPerfLevel=SustainedHigh
FoveatedRenderingMethod=FixedFoveatedRendering
FoveatedRenderingLevel=Off
bDynamicFoveatedRendering=True
bSupportEyeTrackedFoveatedRendering=False
bFocusAware=True
bLateLatching=False
HandTrackingSupport=HandsOnly
HandTrackingFrequency=HIGH
HandTrackingVersion=Default
bInsightPassthroughEnabled=False
bAnchorSupportEnabled=False
bSceneSupportEnabled=False
bBodyTrackingEnabled=False
bEyeTrackingEnabled=False
bFaceTrackingEnabled=False
bDeploySoToDevice=False
```

---

## 四、Android 打包设置

```ini
[/Script/AndroidRuntimeSettings.AndroidRuntimeSettings]
PackageName=com.YourStudio.BlueHandCommand
StoreVersion=1
StoreVersionStr=1.0.0
TargetSDKVersion=34
Orientation=Landscape
InstallLocation=Auto
bPackageForMetaQuest=True
bUseExternalFilesDir=True
bPublicLogFiles=False
bShowLaunchImage=True
bBuildForES31=False
bSupportsVulkanSM5=False
ExtraApplicationSettings=<meta-data android:name="com.oculus.supportedDevices" android:value="quest|quest2|questpro|quest3|quest3s" />
```

> **关于签名**：打包时需要配置 KeyStore，Tiny Trouble 用的是：
> ```
> KeyStore=e:\Tiny Trouble VR\TinyTrouble\Config\AndroidDistribution.keystore
> KeyAlias=TinyTrouble
> ```
> 你需要为自己的项目生成一个新的 keystore。

---

## 五、DefaultDeviceProfiles.ini

```ini
[Oculus_Quest DeviceProfile]
DeviceType=Android
BaseProfileName=Android_Mid
+CVars=xr.VRS.DynamicFoveation=1
+CVars=r.Android.DisableVulkanSupport=0
+CVars=fx.NiagaraAllowGPUParticles=0
+CVars=FX.AllowGPUSorting=0
+CVars=r.Vulkan.VRSFormat=3
+CVars=r.MobileContentScaleFactor=1.2
+CVars=sg.AntiAliasingQuality=3
```

---

## 六、DefaultGame.ini

```ini
[/Script/EngineSettings.GeneralProjectSettings]
ProjectID=你的项目ID（生成一个GUID）
bStartInVR=True
```

---

## 七、与 Tiny Trouble 的关键差异说明

我们故意改了 **2 处**，其余完全对齐：

| 项目 | Tiny Trouble | Blue-Hand-Command | 原因 |
|------|------------|-------------------|------|
| **Bloom** | `True` | `False` ❌ | 关闭 Bloom，省性能 |
| **HeterogeneousVolumes** | `True` | `False` ❌ | 关闭体积雾效果 |
| 动态 FFR | 运行中 `False`，设备配置 `True` | 同样 | 设备配置里启用了动态 FFR |
| `bAutoEnabled` | `False` | 同样先 `False` | Oculus XR 插件通过蓝图手动激活 |

---

## 八、性能 Checklist（对齐 Tiny Trouble）

- [ ] **Lumen** 关闭 ✅（`r.DynamicGlobalIlluminationMethod=0`）
- [ ] **Nanite** 关闭 ✅（`r.Nanite.ProjectEnabled=False`）
- [ ] **VSM** 关闭 ✅（`r.Shadow.Virtual.Enable=0`）
- [ ] **光追** 关闭 ✅
- [ ] **Substrate** 关闭 ✅
- [ ] **Mobile HDR** 关闭 ✅
- [ ] **Mobile Multi-View** 开启 ✅
- [ ] **MSAA 4x** ✅
- [ ] **遮挡剔除** 开启 ✅（Tiny Trouble 开了 `r.AllowOcclusionQueries=True`）
- [ ] **手部追踪** HandsOnly + HIGH 频率 ✅
- [ ] **动态 FFR** 设备配置中开启 ✅
- [ ] **像素密度** 0.8~1.2 ✅
