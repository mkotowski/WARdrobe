---
title: "WARdrobe -- Beat Them Up"
author: [Michał Kotowski]
date: "2020-03-06 -- 2020-06-24"
subject: "PBL"
keywords: [PBL, gamedev]
subtitle: "Game Design Document (version 0.3.2)"
lang: "en"
toc-own-page: true
titlepage: true
titlepage-color: "0159a4"
titlepage-text-color: "ffdb00"
titlepage-rule-color: "ffdb00"
titlepage-rule-height: 2
colorlinks: true
logo: "vga.pdf"
logo-width: 120
header-includes: |
    \usepackage{xcolor}
...

\pagebreak

# Revision List

| Version | Author            | Date       | Comments                           |
|:--------|:------------------|:-----------|------------------------------------|
| 0.1.0   | Michał Kotowski   | 2020-03-06 | First draft.                       |
| 0.2.0   | Jakub Guzek       | 2020-06-22 | Second draft.                      |
| 0.2.1   | Jakub Guzek       | 2020-06-23 | Updating.                          |
| 0.2.2   | Michał Kotowski   | 2020-06-24 | Fix and update metadata.           |
| 0.3.0   | Michał Kotowski   | 2020-06-24 | Add enemies' images.               |
| 0.3.1   | Michał Kotowski   | 2020-06-24 | Update info about controls.        |
| 0.3.2   | Michał Kotowski   | 2020-06-24 | Add tmp team logo.                 |
| 0.3.3   | Łukasz Gołygowski | 2020-06-24 | Add collisions and gui info        |

---

\begin{center}
We throw in extra parts just to mess with you.
-- IKEA, probably
\end{center}

\pagebreak

# Introduction

This document specifies the design guidelines for the gameplay and art style of 
the game **"WARdrobe -- Beat Them Up"**.

## Scope

This document is intended to be read by programmers and artists involved 
in the design, implementation and testing of *"WARdrobe - Beat Them Up"*.

## Type Conventions

Things which have been discussed in a meeting are presented in this document 
using text of normal formatting, like this.

::: proposition
Things which have not been officially agreed on but which are suggested 
by the author are presented in special formatting, like this.
:::

# Team
The name of our team is **Video Game Assembly Studio**.

![f](./img/team_logo/DE15_Connector_Pinout.svg){style="width:70%;margin:auto;"}

- Michał Kotowski
  - leader
  - CMake configuration
  - game's installer configuration
  - GLFW window management
  - GUI placement system
  - GDD
- Jakub Guzek
  - asset creator
  - animation and rendering
  - GDD
- Łukasz Gołygowski
  - AI logic
  - Lua scripts
  - GUI logic
  - collisions

# Core Concepts

## Working Title

~~The current working title --- *Ikea Fighters* --- will be changed 
at a further date to avoid trademark issues.~~

After further development the team had choose the following name:
*"WARdrobe - Beat Them Up"*.

## Elevator Pitch

::: proposition
Fight your way through the waves of aggresive furniture to reach 
the supermarket's checkout, all of it while trying not to run out of drugs.

Wywalcz sobie drogę przez falę agresywnych mebli, aby dotrzeć 
do kasy w supermarkecie, wszystko to, zanim skończą ci się narkotyki.
:::

## Genres
Our game is designed around conventions of simulation and brawler genres.

### Themes
Furniture and psychedelics.

## Target Audience
The target audience of this game will be fans of beat 'em up genre.

## Unique Selling Points
The USP of the game should be the humorous mix of "living" furniture 
and visual effects inspired by effects of psychedelics.

## Target Platforms
The targeted platforms are:

- Windows 10
- Ubuntu 20.04

# References

## Beat 'em up (brawler) genre
From Wikipedia:

> [Beat 'em up][wikibeatemup] is a video game genre featuring hand-to-hand 
combat between the protagonist and an improbably large number of opponents.

## Dynasty Warriors
*Dynasty Warriors* by Omega Force and Koei is a series of hack and slash 
action video games.

# Specification

## Concept
The aim of 

## Story
Once upon a time, there was a furniture shop with... eh... 
Its kinda bland isn't it? Ok, let me do it again.
Imagine hangover so hard that you landed in the backroom of furniture shop 
with monsters in it. Weird huh? What if those monsters are also furnitures? 
Add to that a(n) (un)healthy dose of drugs and here is your situation. 
Will you fight your way out? Will you survive that amount of drugs? 
Will you discover what is going on in the shop?

### Setting
The game will be set in a generic furniture shop in a supermarket. 
Enemies are living furnitures and pickups are drugs.

## Game Structure
Player starts in small room and travels from one room to another. 
There are no other rooms outside those on main path. 
Player is fighting mainly on arenas, bigger areas where there are 
more enemies and in corridors/hallways, that are smaller and have less enemies. 
There are also hidden power Ups in the corridor section. 
Each Arena is divided by corridor. 
Final two rooms are designed as boss arenas with room before it 
being empty space with few pill preparing player for final fight.

## Player's character
Player's character is a drug addict, also reffered as **Drugman**, 
that woke up in the furniture shop.

Player can attack with both hands separately as well as making *combos*.
 
Player has to manage not only *their own health*, but 
also *health of their brain*, that will become more and more poisoned 
from taking pills.

## Action
As mentioned before, player can attack with both hands separately as well 
as making combos. There are two special attacks that are made 
after finishing combos:

- Windmill
  - combo: hand 1 strong attack, hand 2 weak attack, hand 1 weak attack
  - AoE attack that pushes off enemies in the nearby area.
- Dash
  - combo: hand 1 weak attack, hand 1 weak attack, hand 2 strong attack 
  - player dashes in straight line dealing extensive damage to enemies

Player has also to theirs disposal drugs, often reffered to as pills. 
There are three types of pills:

- Damage(red) -- increases players damage for a short while
- Speed(blue) -- increases players speed for a short while
- Health(green) -- heals player

Pills will deal brain damage that will heal with time, but overdose 
(indicator is more then 100) will hurt also "normal" health.

Pills are stored in the inventory (2 max)

## Objective
The objective of the player is to get out from the shop and to do it, 
player has to fight his way through enemies. 
Their final obstacle is boss that guards the exit 
and will not let player to escape that easily.

## Graphics
Models and textures are fairly simple, made using *MagicaVoxel* 
and animated using *Blender*. All of it is filled with special 
coloring of the meshes when player uses the pill.

## Data Storage
Levels are stored in .json files, that are then parsed. 
Inside it are entitities with corresponding components. 
Models are stored in .obj and .fbx files, 
animations are stored in .fbx files, textures are stored in .png files. 

# Gameplay

## World
Generic furniture shop with a large warehouse filled with crates, 
shelves and forklifts. Also pills.

## Object Types

1. Obstacles - typical obstacles that you can't move through: 
  - shelves, 
  - boxes, 
  - crates, 
  - forklifts,
  - ...
2. Pills - power-ups that helps player through the level
  - green healing pill
  - red attack-enhanting pill
  - blue speed-enchanting pill

\pagebreak

## NPC Types (Enemies)

### Bat Bulb
A common enemy, that attacks the player directly, 
it's a lightbulb with winds

![Bat Bulb's model](./img/enemies/BatBulb_export_transparent.png)

\pagebreak

### Lamp Monster
A common enemy, that attacks the player directly,.
It's a lamp, that jumps at you and smacks you with its teeth.

![Lamp Monster's model](./img/enemies/Lamp_Monster_export_transparent.png)

\pagebreak

### Mayor Plywoodov
Boss, a big wardrobe, that commands other enemies.

---

### Unused designs
The list of abandoned/on hold designs:

- Carabinet

![Carabinet's model](./img/enemies/Carabinet_export_transparent.png)

## Controls

### Keyboard and mouse (KB+M)

- WASD, arrow keys -- player movement
- left mouse button -- left hand
- right mouse button -- right hand
- cursor movement -- turning around
- Q, E, 4th and 5th mouse button -- use pill (left or right pill in inventory)

Hold button/key for a strong attack.

### Gamepads

The basic support for gamepads using the standard mapping, which include:

- Microsoft Xbox 360 Controller
- Microsoft Xbox One Controller

Microsoft's controllers are consodered the standard for PC games 
therefore should be the main focus.

---

In this document and all of the documentation

[][w3cpad]

![W3C Standard Gamepad](./img/gamepad/standard_gamepad.svg)

::: proposition

| Control        | Action         |
|:--------------:|:---------------|
| L              | Lock On        |
| ZL             | Guard          |
| X              | Regular Attack |
| Left Joystick  | Move           |
| Right Joystick | Camera Control |

Based on controls of the game *Fire Emblem Warriors*.

:::

## Physics

Physics system is modifying objects world position based on velocity from it's RigidBody component.
Velocity can be changed by applying impulse force which changes RigidBody's acceleration or setting
it's value directly. The system is gradually decreasing the velocity's value.
The position is then corrected by collisions system.

### Collisions

We are using OBBs (Oriented Bounding Boxes) as we needed to detect collisions 
between rotating objects. To detect and resolve collisions we implemented
SAT (Separating Axis Theorem).

# Front End

GUI elements:

- Face indicating player's health or if he was attacked

![Full health](./img/gui/Face.png)

![Player attacked](./img/gui/HitEffect.png)

![Low health](./img/gui/FaceBadHealth.png)

- Indicators that pill is influencig player's statistics

![Damage increased](./img/gui/RedPillEffect.png)

![Speed increased](./img/gui/BluePillEffect.png)

![Health increased](./img/gui/GreenPillEffect.png)

## Intro

## Menus

# Development Tools

The development process will be focused on compatibility with Windows 10.
The use of cross-platform tools enables our team to test the game under 
Linux-based operating system at a further date.

For achieving the stage of *Minimal Viable Product*, the team will focus 
on developing the game for Windows 10, but 
**any Windows-specific solution should be avoided**.

~~If the progress of the game will be satisfactory, the testing of Linux support 
could be considered.~~

## Programming Languages, Standards and APIs

- C++11
- OpenGL -- a cross-platform API for rendering 2D and 3D vector graphics
- OpenAL -- a cross-platform audio API

## Libraries and frameworks

- Assimp -- an API for loading 3D assets, including both models and animations
- nlohmann/json  -- a library used to parse .json files

## Build automation

- [CMake][cmake]

## Version Control System

- Git
- GitHub

## Editors

The code is expected to by developed mostly 
in *Microsoft Visual Studio 2019*, but should be IDE-independend.

## Debugging

### OpenGL

- [RenderDoc][renderdoc] -- graphics debugger

---

## Game Design Document creation pipeline

This document was created with use of the following:

- [Pandoc’s Markdown][pandocmd] syntax
- [Pandoc][pandoc] -- Pandoc 2.9.2-1 amd64 for Ubuntu
- [Eisvogel][eisvogel] -- Pandoc LaTeX template (BSD 3-Clause)

Pandoc's Markdown extensions used:

- `fancy_lists`
- `task_lists`
- `fenced_divs`
- `definition_lists`

# Timeline

- Prototyping stage -- 2020-03-05 - 2020-03-26
- Elaboration stage -- 2020-03-27 - 2020-04-16
- Construction stage
- Transition stage
- ZTGK submission deadline -- 2020-06-26

---

# Glossary

[cmake]: https://cmake.org/
[renderdoc]: https://renderdoc.org/
[pandoc]: https://pandoc.org/
[pandocmd]: https://pandoc.org/MANUAL.html
[eisvogel]: https://github.com/Wandmalfarbe/pandoc-latex-template
[wikibeatemup]: https://en.wikipedia.org/wiki/Beat_%27em_up
[wikihacknslash]: https://en.wikipedia.org/wiki/Hack_and_slash
[shadered]: https://shadered.org/
[w3cpad]: https://www.w3.org/TR/gamepad/
