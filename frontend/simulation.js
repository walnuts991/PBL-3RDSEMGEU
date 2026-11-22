
// ============================================
// GRIDGUARD - POWER GRID CONTROL ROOM
// Interactive visual simulation
// ============================================

const svg = document.getElementById("network-svg");
const NS = "http://www.w3.org/2000/svg";

// Sample network matching your GridGuard test data
const nodes = [
    { id: 1, name: "North", type: "generator", x: 100, y: 205 },
    { id: 2, name: "Central", type: "substation", x: 300, y: 105 },
    { id: 3, name: "South", type: "substation", x: 500, y: 205 },
    { id: 4, name: "East", type: "load", x: 700, y: 105 },
    { id: 5, name: "West", type: "load", x: 700, y: 345 }
];

const lines = [
    { id: 1, from: 1, to: 2, capacity: 100 },
    { id: 2, from: 2, to: 3, capacity: 120 },
    { id: 3, from: 1, to: 3, capacity: 80 },
    { id: 4, from: 3, to: 4, capacity: 90 },
    { id: 5, from: 3, to: 5, capacity: 50 }
];

let failedLines = new Set();
let guidedMode = false;

// --------------------------------------------
// SVG ELEMENT HELPER
// --------------------------------------------

function svgElement(tag, attributes = {}) {
    const element = document.createElementNS(NS, tag);

    Object.entries(attributes).forEach(([key, value]) => {
        element.setAttribute(key, value);
    });

    return element;
}

// --------------------------------------------
// EVENT LOG
// --------------------------------------------

function logEvent(message, type = "normal") {
    const log = document.getElementById("event-log");
    const item = document.createElement("p");

    item.textContent = message;
    item.className = type === "fault"
        ? "fault-event"
        : "normal-event";

    log.prepend(item);

    while (log.children.length > 8) {
        log.lastElementChild.remove();
    }
}

// --------------------------------------------
// DRAW TRANSMISSION NETWORK
// --------------------------------------------

function drawLines() {
    svg.querySelectorAll(".line-layer").forEach(
        element => element.remove()
    );

    lines.forEach(line => {
        const from = nodes.find(n => n.id === line.from);
        const to = nodes.find(n => n.id === line.to);

        const failed = failedLines.has(line.id);

        const group = svgElement("g", {
            class: "line-layer"
        });

        const color = failed ? "#ef4444" : "#38bdf8";

        // Soft glowing line
        const glow = svgElement("line", {
            x1: from.x,
            y1: from.y,
            x2: to.x,
            y2: to.y,
            stroke: color,
            "stroke-width": 10,
            opacity: failed ? 0.10 : 0.15
        });

        // Main transmission line
        const wire = svgElement("line", {
            x1: from.x,
            y1: from.y,
            x2: to.x,
            y2: to.y,
            stroke: color,
            "stroke-width": 2.5,
            "stroke-dasharray": failed ? "8 7" : "5 6",
            class: "transmission-wire",
            style: "cursor:pointer"
        });

        wire.addEventListener("click", () => {
            if (guidedMode) return;

            if (failed) {
                restoreLine(line.id);
            } else {
                failLine(line.id);
            }
        });

        group.append(glow, wire);

        // Animated electricity
        if (!failed) {
            const dot = svgElement("circle", {
                r: 4,
                fill: "#facc15",
                filter: "url(#electric-glow)"
            });

            const motion = svgElement("animateMotion", {
                dur: `${2.5 + line.id * 0.3}s`,
                repeatCount: "indefinite",
                path: `M ${from.x} ${from.y} L ${to.x} ${to.y}`
            });

            dot.appendChild(motion);
            group.appendChild(dot);
        }

        // Transmission line ID
        const label = svgElement("text", {
            x: (from.x + to.x) / 2,
            y: (from.y + to.y) / 2 - 9,
            fill: failed ? "#f87171" : "#7893ae",
            "font-size": 10,
            "text-anchor": "middle"
        });

        label.textContent = `L${line.id}`;

        group.appendChild(label);
        svg.appendChild(group);
    });
}

// --------------------------------------------
// DRAW SUBSTATIONS
// --------------------------------------------

function drawNodes() {
    svg.querySelectorAll(".node-layer").forEach(
        element => element.remove()
    );

    nodes.forEach(node => {
        const group = svgElement("g", {
            class: "node-layer"
        });

        const affected = lines.some(line =>
            failedLines.has(line.id) &&
            (line.from === node.id || line.to === node.id)
        );

        let color = "#38bdf8";

        if (node.type === "generator") color = "#22c55e";
        if (node.type === "load") color = "#f59e0b";
        if (affected) color = "#ef4444";

        // Outer glow
        const outer = svgElement("circle", {
            cx: node.x,
            cy: node.y,
            r: 31,
            fill: color,
            opacity: 0.09
        });

        // Main node
        const circle = svgElement("circle", {
            cx: node.x,
            cy: node.y,
            r: 20,
            fill: "#10233a",
            stroke: color,
            "stroke-width": 2.5
        });

        // Inner point
        const inner = svgElement("circle", {
            cx: node.x,
            cy: node.y,
            r: 5,
            fill: color
        });

        // Node ID
        const number = svgElement("text", {
            x: node.x,
            y: node.y + 4,
            fill: "#ffffff",
            "font-size": 12,
            "font-weight": "bold",
            "text-anchor": "middle"
        });

        number.textContent = node.id;

        // Node name
        const label = svgElement("text", {
            x: node.x,
            y: node.y + 39,
            fill: "#cbd5e1",
            "font-size": 12,
            "text-anchor": "middle"
        });

        label.textContent = node.name;

        group.append(outer, circle, inner, number, label);
        svg.appendChild(group);
    });
}

// --------------------------------------------
// SVG GLOW EFFECT
// --------------------------------------------

function addGlow() {
    const defs = svgElement("defs");
    const filter = svgElement("filter", {
        id: "electric-glow",
        x: "-100%",
        y: "-100%",
        width: "300%",
        height: "300%"
    });

    const blur = svgElement("feGaussianBlur", {
        stdDeviation: 3,
        result: "blur"
    });

    filter.appendChild(blur);
    defs.appendChild(filter);
    svg.appendChild(defs);
}

// --------------------------------------------
// UPDATE STATISTICS
// --------------------------------------------

function updateStats() {
    const active = lines.length - failedLines.size;

    const health = Math.round(
        (active / lines.length) * 100
    );

    document.getElementById("station-count").textContent =
        nodes.length;

    document.getElementById("failed-lines").textContent =
        failedLines.size;

    document.getElementById("network-health").innerHTML =
        `${health}<small>%</small>`;

    document.getElementById("power-flow").innerHTML =
        failedLines.size === 0
            ? '180 <small>MW</small>'
            : '-- <small>MW</small>';

    document.getElementById("frequency").innerHTML =
        failedLines.size === 0
            ? '50.00 <small>Hz</small>'
            : '49.80 <small>Hz</small>';

    document.getElementById("outage-status").textContent =
        failedLines.size === 0
            ? "No active faults"
            : "Transmission fault detected";

    document.getElementById("outage-status").className =
        failedLines.size === 0 ? "green-text" : "";

    document.getElementById("grid-status").textContent =
        failedLines.size === 0
            ? "NORMAL OPERATION"
            : "FAULT DETECTED";

    document.getElementById("grid-status").style.color =
        failedLines.size === 0 ? "#22c55e" : "#ef4444";

    updateStationList();
}

// --------------------------------------------
// UPDATE LEFT SUBSTATION PANEL
// --------------------------------------------

function updateStationList() {
    const container = document.getElementById("station-list");

    container.innerHTML = "";

    nodes.forEach(node => {
        const affected = lines.some(line =>
            failedLines.has(line.id) &&
            (line.from === node.id || line.to === node.id)
        );

        const item = document.createElement("div");
        item.className = "station-item";

        const statusColor = affected ? "#ef4444" : "#22c55e";

        item.innerHTML = `
            <div>
                <span>${node.name}</span>
                <b style="color:${statusColor}">●</b>
            </div>
            <div class="load-bar">
                <span style="
                    width:${affected ? 30 : 75}%;
                    background:${statusColor};
                "></span>
            </div>
        `;

        container.appendChild(item);
    });
}

// --------------------------------------------
// RENDER NETWORK
// --------------------------------------------

function renderNetwork() {
    drawLines();
    drawNodes();
    updateStats();
}

// --------------------------------------------
// SIMULATE TRANSMISSION LINE FAILURE
// --------------------------------------------

function failLine(id) {
    if (failedLines.has(id)) return;

    failedLines.add(id);

    logEvent(
        `ALERT: Transmission line ${id} has failed!`,
        "fault"
    );

    logEvent(
        "GridGuard detected a network connection failure.",
        "fault"
    );

    renderNetwork();
}

// --------------------------------------------
// RESTORE A TRANSMISSION LINE
// --------------------------------------------

function restoreLine(id) {
    if (!failedLines.has(id)) return;

    failedLines.delete(id);

    logEvent(`Transmission line ${id} restored.`);

    renderNetwork();
}

// --------------------------------------------
// RESTORE COMPLETE NETWORK
// --------------------------------------------

function restoreNetwork() {
    failedLines.clear();

    logEvent("Network restoration completed.");

    renderNetwork();
}

// --------------------------------------------
// BUTTON CONTROLS
// --------------------------------------------

document.getElementById("normal-btn").addEventListener(
    "click",
    () => {
        if (guidedMode) return;

        restoreNetwork();
        logEvent("Grid is operating normally.");
    }
);

document.getElementById("fault-btn").addEventListener(
    "click",
    () => {
        if (guidedMode) return;

        if (failedLines.size > 0) {
            logEvent("Restore the current fault first.", "fault");
            return;
        }

        failLine(4);
    }
);

document.getElementById("restore-btn").addEventListener(
    "click",
    () => {
        if (guidedMode) return;

        restoreNetwork();
    }
);

// --------------------------------------------
// GUIDED SIMULATION
// --------------------------------------------

function wait(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

document.getElementById("guided-btn").addEventListener(
    "click",
    async () => {
        if (guidedMode) return;

        guidedMode = true;

        const button = document.getElementById("guided-btn");
        button.textContent = "Simulation Running...";
        button.style.opacity = "0.6";

        restoreNetwork();

        logEvent("GUIDED SIMULATION STARTED");

        await wait(2000);

        logEvent("STEP 1: Normal electricity flow.");

        await wait(2500);

        logEvent("STEP 2: A transmission line failure occurs.");

        failLine(4);

        await wait(2500);

        logEvent("STEP 3: GridGuard identifies the failed connection.");

        await wait(2500);

        logEvent("STEP 4: Restoring the network.");

        restoreNetwork();

        await wait(2000);

        logEvent("GUIDED SIMULATION COMPLETED.");

        guidedMode = false;
        button.textContent = "▶ Guided Simulation";
        button.style.opacity = "1";
    }
);

// --------------------------------------------
// INITIALIZE SIMULATION
// --------------------------------------------

addGlow();
renderNetwork();

logEvent("GridGuard monitoring system initialized.");
logEvent("All transmission lines are operational.");
