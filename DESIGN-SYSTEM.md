# Proyecto Hugo — Sistema de Diseño v4.0 "Latido"

> Documento de referencia del rediseño visual del sitio. Sirve como base para futuras mejoras, cambios de contenido o nuevas páginas, manteniendo coherencia visual.

---

## 1. Dirección y concepto

**Dirección elegida:** Moderno vibrante (gradientes, glassmorphism, estética "startup").

**Concepto central:** *"Medir con amor, cuidar con datos."* El dispositivo es, literalmente, un monitor de signos vitales hecho con Arduino. Todo el sistema visual gira alrededor de esa idea: fusionar la estética de un instrumento de laboratorio/monitor médico con la calidez de una historia de amor real.

**Elemento de firma (signature element):** un panel de vidrio oscuro tipo "instrumento" con:
- Pantalla LCD simulada con lecturas en vivo (salinidad, temperatura, pH)
- Una línea tipo electrocardiograma animada (`wave-line`, color coral)
- Punto "live" pulsante

Este panel aparece en el hero de la home y se referencia en el Monitor real (`Monitor/index.html`). No usarlo en exceso fuera de esos dos lugares: es el elemento que debe recordarse, el resto del sitio debe ser más quieto.

---

## 2. Paleta de colores

| Token | Hex | Uso |
|---|---|---|
| `--teal` | `#0F9D8C` | Color primario — ciencia, agua, salud. Botones principales, links, iconos. |
| `--teal-glow` | `#34E0C4` | Acento de gradientes junto a `--teal`. Resplandores, highlights de texto. |
| `--coral` | `#FF6F91` | Color humano — amor, historia, dedicatoria. Corazones, citas, acentos emocionales. |
| `--coral-deep` | `#E84F77` | Variante oscura de coral para texto sobre fondos claros (mejor contraste). |
| `--amber` | `#FFB648` | Color del "Potasio (K⁺)" — todo lo relacionado a la novedad/expansión del proyecto. |
| `--navy` | `#0A1628` | Fondo oscuro — hero, paneles de instrumento, footer. |
| `--mist` | `#F4FAF9` | Fondo claro general del sitio. |
| `--ink` | `#0B1220` | Texto principal sobre fondo claro. |
| `--slate` | `#4B5A6A` | Texto secundario / párrafos de cuerpo. |
| `--card` | `#FFFFFF` | Fondo de tarjetas en modo claro. |
| `--line` | `rgba(11,18,32,0.08)` | Bordes sutiles. |

**Modo oscuro** (atributo `data-theme="dark"` en `<html>`, implementado en `index.html`):
- `--mist: #0B1422` · `--card: #101D30` · `--ink: #EAF3F1` · `--slate: #C7D4DD`

**Regla de uso:** teal = lo técnico/científico. Coral = lo humano/emocional. Ámbar = lo nuevo (potasio). Navy = momentos "instrumento" (hero, paneles, footer). No mezclar coral y ámbar en el mismo componente salvo en el footer/dedicatoria.

---

## 3. Tipografía

| Rol | Fuente | Pesos usados | Dónde |
|---|---|---|---|
| Display / títulos | **Space Grotesk** | 500, 600, 700 | `h1, h2, h3`, logo, badges |
| Cuerpo | **Inter** | 400, 500, 600, 700 | Párrafos, navegación, botones |
| Datos / técnico | **JetBrains Mono** | 400, 700 | Lecturas de sensores, specs, reloj del panel |

Import usado en todas las páginas (Google Fonts):
```
https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@500;600;700&family=Inter:wght@400;500;600;700&family=JetBrains+Mono:wght@400;500;700&display=swap
```

**Por qué Space Grotesk:** reemplaza a "Outfit" de la v3 anterior. Es más geométrica y técnica, encaja con el costado "instrumento de laboratorio / startup" sin perder calidez en minúsculas.

---

## 4. Layout y componentes clave

### Tokens de espaciado/forma
- `--radius: 20px` (tarjetas grandes) · `--radius-sm: 12px` (tarjetas chicas/notices)
- `--ease: cubic-bezier(.22,1,.36,1)` — easing usado en todas las transiciones/hovers
- `--shadow-card: 0 18px 40px -16px rgba(10,22,40,0.16)` — sombra estándar de tarjetas
- `--shadow-glow: 0 0 0 1px rgba(255,255,255,.06), 0 30px 80px -20px rgba(15,157,140,.45)` — sombra del panel-instrumento

### Componentes reutilizables (ver CSS embebido en cada página)
- **`.eyebrow`** — etiqueta pill mayúscula antes de cada título de sección
- **`.feature-card`** — tarjeta blanca con ícono en chip de color + título + texto (usada en "Cómo funciona" y "Por qué importa")
- **`.panel` / `.lcd` / `.wave-line`** — el instrumento/signature element
- **`.version-card`** — tarjeta de cada rama de código (main, VersionAgosto2025, potasio)
- **`.story-card`** — bloque oscuro con gradiente para el teaser de Historia
- **`.k-trigger`** — botón flotante ámbar de "Novedad: Potasio"
- **`.insta-sidebar`** — panel lateral deslizable de Instagram (se mantiene de la v3)
- **`#hearts-container`** — corazones flotantes ambientales (se mantiene de la v3, ahora en color coral)

### Estructura de navegación del sitio
```
index.html (home)
 ├─ #inicio        → hero
 ├─ #funciona      → Componentes / hardware
 ├─ #vision        → Por qué importa
 ├─ historia/      → página dedicada (NUEVA)
 ├─ #versiones     → main/ y VersionAgosto2025/ + link a rama potasio (GitHub)
 ├─ #autora        → Paulina Juich
 └─ Monitor/       → dashboard en vivo (Tailwind + Chart.js)
```

---

## 5. Decisión de contenido importante

La **historia personal** (dedicatoria a Hugo Passucci + carta de amor a Agus Nicolás) se movió a una **página separada** (`historia/index.html`) en vez de vivir mezclada en la sección "Visión" de la home, como estaba en la v3. Esto fue una decisión explícita del cliente:

> Mantener la historia completa, pero en su propio espacio — para que la home se sienta profesional/técnica a primera vista, y quien quiera profundizar en la historia humana pueda hacerlo en un lugar dedicado y con más fuerza narrativa (timeline + carta completa + dedicatoria).

La home (`index.html`) solo deja un **teaser emocional** (`.story-card`) con una cita corta y un botón "Leer la historia completa →".

---

## 6. Mapa de archivos del sitio

```
/
├── index.html                  → Home (nuevo diseño v4.0)
├── historia/index.html         → Historia/Origen (página nueva)
├── main/index.html             → Rama "main" (reskineada)
├── VersionAgosto2025/index.html→ Rama experimental (reskineada)
├── Monitor/index.html          → Dashboard en vivo (reskineado, Tailwind+Chart.js)
├── README.md                   → Sin cambios (licencia, dedicatoria, autora)
├── LICENSE
└── Informe de Progreso Anual 2025 - Proyecto Hugo.pdf
```

Cada página tiene su CSS embebido (no hay hoja de estilos compartida todavía — ver sección 8, "Próximos pasos").

---

## 7. Funcionalidades conservadas de la v3

- 🌙 Modo oscuro con `localStorage` (`index.html` únicamente por ahora)
- 💚 Corazones flotantes animados
- 📸 Sidebar de Instagram deslizable
- 💛 Solapa/botón flotante de "Novedad: Potasio"
- 📊 Dashboard del Monitor con Chart.js, simulación de lecturas en tiempo real, toggle Salinidad/Potasio
- Aviso legal: *"Este proyecto no constituye un dispositivo médico ni realiza diagnósticos clínicos."*

---

## 8. Próximos pasos / ideas para iterar

- [ ] Extraer el CSS común a un solo archivo `assets/proyecto-hugo.css` para no repetirlo en cada página (hoy está embebido por página para que cada archivo se pueda previsualizar solo).
- [ ] Llevar el dark mode a todas las páginas (hoy solo está en `index.html`).
- [ ] Sumar fotos reales del prototipo armado (actualmente todo es iconografía SVG).
- [ ] Posible sección de "Cómo armar el tuyo" (guía paso a paso / BOM de componentes).
- [ ] Cuando el medidor de Potasio tenga su propia rama estable, podría merecer su propia mini-página tipo `main/` y `VersionAgosto2025/`, en vez de solo linkear a GitHub.
- [ ] Evaluar agregar Open Graph / meta tags para que el link se vea bien al compartirlo en redes (Instagram, WhatsApp).

---

## 9. Carrusel de fotos estilo story (panel Instagram)

Agregado dentro del panel lateral de Instagram (`.insta-sidebar`), justo arriba de la tarjeta "Seguí el desarrollo". No reemplaza nada del panel original (ver sección 5/6) — el resto del panel sigue intacto.

- **Comportamiento:** automático, cambia de foto cada 4 segundos, con barritas de progreso arriba estilo Instagram Stories. Se puede pasar manualmente tocando la mitad izquierda/derecha de la imagen. Se pausa al pasar el mouse por encima (desktop).
- **Fotos usadas:** 5 fotos reales del proyecto, guardadas en `assets/carousel/foto-1.jpg` a `foto-5.jpg` (formato vertical/cuadrado, `aspect-ratio: 4/5`, `object-fit: cover`).
- **Componente:** `.insta-carousel`, `.story-bars`, `.story-slide`, `.story-nav` — nombres elegidos para no chocar con ninguna clase del bloque original restaurado.
- **JS:** función autoejecutable al final del `<script>` de `index.html`, separada de la lógica del panel Potasio/Instagram para no modificarla.

### Mapa de archivos actualizado
```
/
├── assets/carousel/foto-1.jpg … foto-5.jpg   → fotos del carrusel (NUEVO)
├── index.html
├── historia/index.html
├── main/index.html
├── VersionAgosto2025/index.html
├── Monitor/index.html
├── README.md
├── LICENSE
└── Informe de Progreso Anual 2025 - Proyecto Hugo.pdf
```

### Próximos pasos para el carrusel
- [ ] Sumar más fotos a medida que haya nuevo contenido (solo agregar `foto-6.jpg`, etc. y su `<div class="story-slide">` correspondiente).
- [ ] Si se quiere evitar editar HTML a mano cada vez, se puede migrar a un array JS que genere los slides dinámicamente.
- [ ] Posible swipe táctil nativo (hoy el tap funciona, pero no hay gesto de arrastre).

---

*Documento generado junto con el rediseño v4.0. Última actualización: 30 de junio de 2026.*
