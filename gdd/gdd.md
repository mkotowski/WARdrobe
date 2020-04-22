---
title: "IKEA Fighters GDD"
author: [Michał Kotowski]
date: "2020-03-06"
subject: "PBL"
keywords: [PBL, gamedev]
subtitle: "Game Design Document (version 0.1.0)"
lang: "en"
toc-own-page: true
titlepage: true
titlepage-color: "0159a4"
titlepage-text-color: "ffdb00"
titlepage-rule-color: "ffdb00"
titlepage-rule-height: 2
colorlinks: true
header-includes: |
    \usepackage{xcolor}
...

\pagebreak

# Revision List

| Version | Author          | Date       | Comments                           |
|:--------|:----------------|:-----------|------------------------------------|
| 0.1.0   | Michał Kotowski | 2020-03-06 | First draft.                       |

---

::: warning
Draft document!
:::

---

\begin{center}
We throw in extra parts just to mess with you.
-- IKEA, probably
\end{center}

\pagebreak

# Introduction

This document specifies a design for the gameplay of a game with 
the provisional title *"Ikea Fighters"*.

## Scope

This document is intended to be read by programmers and artists involved 
in the design, implementation and testing of *Ikea Fighters*.

## Type Conventions

Things which have been discussed in a meeting are presented in this document 
using text of normal formatting, like this.

::: proposition
Things which have not been officially agreed on but which are suggested 
by the author are presented in special formatting, like this.
:::

# Core Concepts

## Working Title

The current working title --- *Ikea Fighters* --- will be changed 
at a further date to avoid trademark issues.

::: proposition
Current propositions for the permanent name:

- ...
:::

## Elevator Pitch

::: proposition
Fight your way through the waves of aggresive furniture to reach 
the supermarket's checkout, all of it while trying not to run out of drugs.

Wywalcz sobie drogę przez falę agresywnych mebli, aby dotrzeć 
do kasy w supermarkecie, wszystko to, zanim skończą ci się narkotyki.

:::proposition
The aspect of running out of drugs -- yes or not?
:::

:::

## Genres

Simulation and brawler.

### Themes

Furniture and psychedelics.

## Target Audience

The target audience of this game will be fans of beat 'em up genre.

## Unique Selling Points

The USP of the game should be the humorous mix of "living" furniture 
and visual effects inspired by effects of psychedelics.

## Target Platforms

### Windows 10

The Microsoft's system is the most obvious target platform for our game.

---

::: proposition
### Linux
:::

# References

## Beat 'em up (brawler) genre

From Wikipedia:

> [Beat 'em up][wikibeatemup] is a video game genre featuring hand-to-hand 
combat between the protagonist and an improbably large number of opponents.

### Dynasty Warriors

*Dynasty Warriors* by Omega Force and Koei is a series of hack and slash 
action video games.

::: proposition3

:::


# Specification

## Concept

The aim of 

## Story

### Setting

The game will be set in a generic furniture shop in a supermarket.

::: proposition
The generated areas should be generated procedurally from a small 
set of premade elements.
:::

## Game Structure

## Players

## Action

## Objective

## Graphics

::: proposition
Due to the lack of artists on the team, the most simplistic textures and 
models possible are to be used.

Extensive use of shaders and particles is expected and should be 
the main focus when developing the game's graphics.
:::

## Data Storage

# Gameplay

## World

## Landscape

## Ground Type

## Object Types

## NPC Types

## Controls

The game should be able to handle the traditional keyboard 
and standard gamepad mapping.

::: proposition

:::

::: warning

:::

### Keyboard and mouse (KB+M)

### Gamepads

The basic support for gamepads:

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

### Collisions

The current plans for gameplay do not demand a very precise collision system.

Since the gameplay demands the ability to handle significant number of enemies, 
a simple collision system should be enough.

### Particle system



# Front End

## Intro

## Menus

# Development Tools

The development process will be focused on compatibility with Windows 10.

::: proposition
For achieving the stage of Minimal Viable Product, the team will focus 
on developing the game for Windows 10, but **any Windows-specific solution 
should be avoided**.

If the progress of the game will be satisfactory, the testing of Linux support 
could be considered.
:::

## Programming Languages, Standards and APIs

- C++11
- OpenGL -- a cross-platform API for rendering 2D and 3D vector graphics
- OpenAL -- a cross-platform audio API

::: propositions

:::

## Build automation

- [CMake][cmake]

## Version Control System

- Git
- GitHub

::: proposition

:::

## Editors

The code is expected to by developed mostly 
in *Microsoft Visual Studio 2019*, but should be IDE-independend.

::: proposition
For the development of shaders the shader editor *[Shadered][shadered]* 
will be used before testing shaders directly in the game.
:::

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

# Team

Programmers:

- Michał Kotowski
- Jakub Guzek
- Piotr Gortyński
- Łukasz Gołygowski

# Time

# Roadmap

## Prototyping stage

2020-03-05 --- 2020-03-26

## Elaboration stage

2020-03-27 --- 2020-04-16

# Construction stage

# Transition stage

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