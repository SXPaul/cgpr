# Marble Maze - OpenGL Final Project

A realistic 3D marble maze game implemented in C++/OpenGL with **Physically Based Rendering (PBR)**.

![Marble Maze Screenshot](docs/screenshot.png)

## Features

- **PBR Rendering** - Cook-Torrance BRDF with GGX distribution
- **Realistic Materials** - Wood textures with normal mapping, metallic chrome ball
- **Grid-based Levels** - 3 built-in levels, easy to add more
- **Simple Physics** - Tilt-based ball movement with wall collision
- **Keyboard Controls** - No mouse required for gameplay

## Quick Start

```bash
# Clone and build
mkdir build && cd build
cmake .. -DUSE_REAL_TEXTURES=ON
make -j4

# Run
./RealisticRenderer
```

## Controls

| Key | Action |
|-----|--------|
| **Arrow Keys** | Tilt the board (control the ball) |
| **WASD** | Pan camera |
| **Q / E** | Orbit camera left/right |
| **Scroll** | Zoom in/out |
| **F** | Reset camera view |
| **R** | Restart current level |
| **N** | Next level (after winning) |
| **ESC** | Quit |

## Project Structure

```
├── src/
│   ├── main.cpp           # Game loop, input handling, rendering
│   ├── Ball.cpp           # Ball physics simulation
│   ├── Level.cpp          # Grid-based level system (loads from files)
│   ├── BoardGenerator.cpp # Convert level grid to 3D meshes
│   ├── Shader.cpp         # GLSL shader management
│   ├── Camera.cpp         # Orbit camera
│   ├── Mesh.cpp           # VAO/VBO handling
│   ├── Primitives.cpp     # Procedural sphere, cube, cylinder等
│   └── Texture.cpp        # Texture loading (stb_image)
├── include/
│   ├── Config.h           # All tunable game parameters
│   └── ...
├── assets/
│   ├── shaders/
│   │   ├── pbr.vert       # PBR vertex shader
│   │   └── pbr.frag       # PBR fragment shader (detailed comments)
│   ├── textures/          # Wood & ball PBR textures (ARM format)
│   └── levels/            # Level definition files (*.txt)
│       ├── level1.txt
│       ├── level2.txt
│       └── level3.txt
└── external/
    ├── imgui/             # Dear ImGui for UI
    └── glm/               # OpenGL Mathematics
```

---

## Physically Based Rendering (PBR) 详解

### 什么是 PBR？

PBR（Physically Based Rendering，基于物理的渲染）是一种通过模拟光与材质的真实物理交互来实现逼真渲染的技术。与传统的 Phong/Blinn-Phong 模型相比，PBR 能更准确地表现金属、木材、塑料等不同材质的视觉特性。

### 核心数学原理

#### 1. 渲染方程 (The Rendering Equation)

$$L_o = \int_\Omega f_r(l, v) \cdot L_i \cdot \cos\theta \, d\omega$$

简化为单光源：

$$L_{out} = f_r(l, v) \cdot L_{in} \cdot (n \cdot l)$$

其中：
- $f_r$ = BRDF（双向反射分布函数）
- $L_{in}$ = 入射光辐照度
- $n$ = 表面法线
- $l$ = 光源方向
- $v$ = 视线方向

#### 2. Cook-Torrance BRDF

$$f_r = k_d \cdot f_{lambert} + k_s \cdot f_{cook-torrance}$$

- **漫反射项**: $f_{lambert} = \frac{albedo}{\pi}$
- **镜面反射项**: $f_{cook-torrance} = \frac{DFG}{4(n \cdot v)(n \cdot l)}$

#### 3. D - 法线分布函数 (Normal Distribution Function)

使用 **GGX/Trowbridge-Reitz** 分布：

$$D(h) = \frac{\alpha^2}{\pi((n \cdot h)^2(\alpha^2 - 1) + 1)^2}$$

其中 $\alpha = roughness^2$, $h = normalize(l + v)$

**代码实现** (`pbr.frag`):
```glsl
float DistributionGGX(vec3 N, vec3 H, float rough) {
    float a = rough * rough;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / max(denom, 0.0001);
}
```

#### 4. F - 菲涅尔方程 (Fresnel Equation)

使用 **Schlick 近似**：

$$F(v,h) = F_0 + (1 - F_0)(1 - v \cdot h)^5$$

$F_0$ 是基础反射率：
- 非金属: $F_0 \approx 0.04$
- 金属: $F_0 = albedo$

**代码实现**:
```glsl
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
```

#### 5. G - 几何函数 (Geometry Function)

使用 **Smith's method** 结合 **Schlick-GGX**：

$$G(n,v,l) = G_{sub}(n,v) \cdot G_{sub}(n,l)$$

$$G_{sub}(n,x) = \frac{n \cdot x}{(n \cdot x)(1-k) + k}$$

其中 $k = \frac{(roughness + 1)^2}{8}$ (直接光照)

**代码实现**:
```glsl
float GeometrySchlickGGX(float NdotV, float rough) {
    float r = (rough + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}
```

#### 6. 能量守恒

$$k_s = F$$

(菲涅尔项即为镜面反射系数)

$$k_d = (1 - k_s) \cdot (1 - metallic)$$

金属材质吸收折射光，因此 `metallic = 1` 时漫反射为 0。

### PBR 材质参数

| 参数 | 范围 | 说明 |
|------|------|------|
| **Albedo** | RGB | 基础颜色（非金属的漫反射颜色，金属的反射颜色） |
| **Metallic** | 0-1 | 0 = 非金属, 1 = 金属 |
| **Roughness** | 0-1 | 0 = 光滑镜面, 1 = 粗糙漫反射 |
| **AO** | 0-1 | 环境光遮蔽（预烘焙的阴影） |

### ARM 贴图

为减少纹理采样次数，常将 AO、Roughness、Metallic 打包到一张贴图的 RGB 通道：
- **R** = Ambient Occlusion
- **G** = Roughness  
- **B** = Metallic

```glsl
if (useARMMap) {
    vec3 arm = texture(armMap, TexCoords).rgb;
    aoVal = arm.r;
    roughnessVal = arm.g;
    metallicVal = arm.b;
}
```

---

## 配置参数

所有可调参数集中在 `include/Config.h`:

```cpp
// 球体物理
constexpr float BALL_GRAVITY = 30.0f;    // 重力加速度
constexpr float BALL_FRICTION = 0.985f;  // 摩擦系数
constexpr float BALL_MAX_SPEED = 20.0f;  // 最大速度

// 倾斜控制
constexpr float MAX_TILT_DEGREES = 10.0f;      // 最大倾斜角度
constexpr float TILT_SPEED_DEGREES = 45.0f;    // 倾斜速度

// 相机
constexpr float CAMERA_PAN_SPEED = 0.008f;     // 平移速度
constexpr float CAMERA_ORBIT_SPEED = 60.0f;    // 旋转速度
```

---

## 添加新关卡

在 `assets/levels/` 目录中创建新的 `.txt` 文件（如 `level4.txt`）：

```
##########
#S.......#
#.##.O##.#
#........#
#.O....OG#
##########
```

关卡文件按字母顺序加载，**无需重新编译**，直接运行游戏即可。

符号说明:
- `#` = 墙壁
- `.` = 地板
- `O` = 洞（掉入失败）
- `S` = 起点
- `G` = 终点

---

## 依赖

- **GLFW 3.3+** - 窗口管理
- **OpenGL 4.1** - 图形 API
- **GLM** - 数学库 (included)
- **Dear ImGui** - UI 库 (included)
- **stb_image** - 图像加载 (included)

---

## 构建选项

| CMake 选项 | 默认值 | 说明 |
|------------|--------|------|
| `USE_REAL_TEXTURES` | OFF | 启用真实 PBR 纹理（需要 assets/textures/） |

---

## 参考资料

- [Learn OpenGL - PBR Theory](https://learnopengl.com/PBR/Theory)
- [Poly Haven](https://polyhaven.com/) - 免费 PBR 纹理
- [Real Shading in Unreal Engine 4](https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf)

---

## License

MIT License - 可自由使用于学习和项目作业。
