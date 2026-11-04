# Project Architecture & Agent Instructions

## Core Architectural Guardrails

### State Separation Mandate
- Initial static API data with no client interactivity: Flask routes with `render_template` (server-side rendering)
- Dynamic client-side API data: Fetch API from vanilla JS frontend with `/api/` endpoints
- Shared UI-only state: Kept in vanilla JavaScript module scope (no global pollution)
- Local component state: Inline JavaScript state variables
- **CRITICAL ANTI-PATTERN: Never mix server-rendered state with client-side cached state in inconsistent ways**

### File Colocation
- C++ source: `src/` (graph.h, graph.cpp, main.cpp) — **never modify**
- Python backend: `web/app.py` — Flask API routes
- HTML templates: `web/templates/index.html` — Bento Grid dashboard
- Data: `data/` (facebook_combined.txt, names.txt) — **never modify**
- Verification: `facebook_verification.xlsx`, `generate_verification_excel.py`

### Hard File-Sizing Constraints
- HTML template (`web/templates/index.html`): Maximum ~400 lines. Extract sections if exceeded.
- Python backend (`web/app.py`): Maximum ~400 lines. Extract modules if exceeded.
- **Absolute Ceiling: NEVER generate any single file exceeding 500 lines of code.**

### C++ Source Integrity
- `src/graph.cpp`, `src/graph.h`, `src/main.cpp` are the DSA project core. **Do not modify.**
- All new presentation work goes in `web/` only.

### Visualization Rules (Bento Grid)
When modifying `web/templates/index.html`:
- Use CSS `display: grid`, never Flexbox for 2D tile layouts
- Gap-to-radius coupling: 16px border-radius pairs with 12px-16px grid gap
- Single concept per card; reading order: metric → title → meta → action
- WCAG 2.1 AA: focus-visible rings, aria-labels on containers, skip navigation
- Dark Graphite palette: canvas #09090b, surface #121215, text #FFFFFF
- Warm Bento accents: #FAD4C0 (primary), #80A1C1 (secondary), #16A34A (success), #D97706 (warning)
