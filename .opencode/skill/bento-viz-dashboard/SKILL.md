name: bento-viz-dashboard
description: Applies Tamara Munzner's What-Why-How visualization taxonomy, mathematical CSS Bento Grid rules, KPI card anatomy, and WCAG 2.1 AA accessibility to the Flask + vanilla HTML social network dashboard.
license: MIT
compatibility: opencode
metadata:
  domain: frontend-visualization
  stack: flask-tailwind-vanilla-js

1. Tamara Munzner's Nested Model & What-Why-How Decomposition
Before writing UI code, map every dashboard widget through three mandatory tiers:

WHAT (Data Abstraction): Specify exact data types and mathematical definitions.
- Total Nodes: count of unique node IDs in facebook_combined.txt
- Total Edges: count of undirected edges
- Avg Degree: (2 * edges) / nodes
- Max/Min Degree: max/min of adjacency list lengths

WHY (Task Abstraction): Define the explicit analytical action.
- Identify: Find a specific person by ID or name
- Compare: Check if two nodes are directly connected
- Summarize: View graph statistics overview
- Filter: Search nodes by partial name match
- Detect Outliers: Find highest-degree nodes via PageRank or degree centrality

HOW (Visual Encoding & Interaction Idiom):
- Single KPI: Delta-first KPI Card (1x1 cell) with metric, title, meta
- Multi-series temporal/comparison: Chart.js bar chart (2x1 hero cell)
- Line-by-line record comparison: HTML table in result boxes (never cards for uniform scanning)
- Network graph: vis-network force-directed layout (2x2 hero cell)

2. Mathematical Bento Grid Specification
CSS Grid Foundation: Always use display: grid. Never Flexbox for 2D tile layouts.
- Desktop (>=900px): grid-template-columns: repeat(4, 1fr)
- Tablet (>=600px): grid-template-columns: repeat(2, 1fr) with grid-auto-rows: minmax(180px, auto)
- Mobile: grid-template-columns: 1fr

Base Unit & Row Height: 8px base spacing, shared base row height multiple.

Legal Footprints (4-Unit Bento on 12-col desktop):
- 1x1 cell: Standard KPI metric card (col-span-3, row-span-1, aspect 1:1)
- 2x1 cell: Wide chart (col-span-6, row-span-1, aspect 2:1)
- 1x2 cell: Vertical list/feed (col-span-3, row-span-2, aspect 1:2)
- 2x2 cell: Hero analytical view (col-span-6, row-span-2, aspect 1:1)

Gap-to-Radius Coupling:
- 16px border-radius pairs with 12px or 16px grid gap
- 24px border-radius pairs with 16px or 20px grid gap

3. Card Restraint & Aesthetic Overrides
Override Generic AI Defaults:
- NO frosted glassmorphism over purple gradients
- NO decorative gradient-blob icons
- NO multiple equal-weight action buttons inside a single card

Single Concept Rule: One conceptual unit and at most one primary action per card.
Reading order: (1) Primary Metric or Visual, (2) Concise Title, (3) Muted Meta, (4) Single Primary Action.

Color Palettes:
- Dark Graphite: Canvas #09090b, Surface #121215, Text #FFFFFF, Border #27272A, Muted #A1A1AA
- Warm Bento: Primary #FAD4C0, Secondary #80A1C1, Success #16A34A, Warning #D97706

Typography: 12px minimum for compact labels, 32px maximum for headings.

4. Mandatory WCAG 2.1 AA Accessibility Checklist
- DOM order MUST match visual CSS Grid reading order
- Never use tabindex > 0
- Full semantic aria-label on outer tile containers (e.g., aria-label="Total Nodes: 4,039 people")
- Contrast >= 4.5:1 normal text, >= 3:1 large text
- Keyboard focus rings: focus-visible outline-2 outline-offset-2 outline-[#0066CC]
- Group grids with 10+ tiles into <section role="region" aria-label="..."> landmarks
- Skip navigation link at top of page
- aria-live="polite" for streaming updates, aria-live="assertive" only for critical alerts
