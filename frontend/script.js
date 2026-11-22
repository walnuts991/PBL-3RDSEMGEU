
// ==========================================
// GRIDGUARD - C++ BACKEND CONNECTED FRONTEND
// ==========================================

const API_URL = "http://127.0.0.1:8080/api/analyze";

const navButtons = document.querySelectorAll(".nav");
const pages = document.querySelectorAll(".page");
const pageTitle = document.getElementById("page-title");

const substations = [
    { id: 1, name: "North", x: 0, y: 0, type: "generator", supply_mw: 200, demand_mw: 0 },
    { id: 2, name: "Central", x: 1, y: 0, type: "load", supply_mw: 0, demand_mw: 100 },
    { id: 3, name: "South", x: 2, y: 0, type: "load", supply_mw: 0, demand_mw: 80 },
    { id: 4, name: "East", x: 3, y: 0, type: "load", supply_mw: 0, demand_mw: 60 }
];

const lines = [
    { id: 1, from: 1, to: 2, distance_km: 10, terrain_factor: 1.2, capacity_mw: 100 },
    { id: 2, from: 2, to: 3, distance_km: 15, terrain_factor: 1.0, capacity_mw: 120 },
    { id: 3, from: 1, to: 3, distance_km: 20, terrain_factor: 1.5, capacity_mw: 80 },
    { id: 4, from: 3, to: 4, distance_km: 8, terrain_factor: 1.0, capacity_mw: 90 }
];

const candidates = [
    { id: 5, from: 4, to: 1, cost: 50 },
    { id: 6, from: 4, to: 2, cost: 60 }
];

const stations = Object.fromEntries(
    substations.map(s => [s.id, s.name])
);

const disabledLines = new Set();

let latestAnalysis = null;


// ==========================================
// PAGE NAVIGATION
// ==========================================

function openPage(pageId) {
    const page = document.getElementById(pageId);
    if (!page) return;

    pages.forEach(p => p.classList.remove("active"));
    navButtons.forEach(b => b.classList.remove("active"));

    page.classList.add("active");

    const activeButton = document.querySelector(
        `.nav[data-page="${pageId}"]`
    );

    if (activeButton) {
        activeButton.classList.add("active");
        pageTitle.textContent =
            activeButton.querySelector("span")?.textContent ||
            activeButton.textContent.trim();
    }

    window.scrollTo({ top: 0, behavior: "smooth" });
}

navButtons.forEach(button => {
    button.addEventListener("click", () => {
        openPage(button.dataset.page);
    });
});

document.querySelectorAll("[data-go]").forEach(button => {
    button.addEventListener("click", () => {
        openPage(button.dataset.go);
    });
});


// ==========================================
// TOAST NOTIFICATIONS
// ==========================================

function showToast(message) {
    const toast = document.getElementById("toast");
    toast.textContent = message;
    toast.classList.add("show");

    clearTimeout(window.gridToastTimer);

    window.gridToastTimer = setTimeout(() => {
        toast.classList.remove("show");
    }, 2600);
}


// ==========================================
// BACKEND REQUEST
// ==========================================

function createRequestBody() {
    const source = Number(
        document.getElementById("path-source").value
    );

    const destination = Number(
        document.getElementById("path-destination").value
    );

    return {
        substations: substations,
        lines: lines.filter(line => !disabledLines.has(line.id)),
        candidates: candidates,
        budget: 100,
        source_id: source,
        destination_id: destination
    };
}

async function analyzeGrid() {
    try {
        const response = await fetch(API_URL, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify(createRequestBody())
        });

        const result = await response.json();

        if (!response.ok) {
            throw new Error(result.error || "Backend analysis failed.");
        }

        latestAnalysis = result;

        renderAnalysis(result);

        return result;

    } catch (error) {
        console.error("GridGuard API error:", error);

        showToast(
            "Backend connection failed. Check that the C++ server is running."
        );

        document.getElementById("path-message").textContent =
            error.message;

        return null;
    }
}


// ==========================================
// RENDER ALL BACKEND RESULTS
// ==========================================

function renderAnalysis(result) {
    renderNetwork(result);
    renderShortestPath(result);
    renderMaximumFlow(result);
    renderMST(result);
    renderFaultTolerance(result);
}


// ==========================================
// NETWORK DISPLAY
// ==========================================

function renderNetwork(result) {
    const network = result.network;

    const activeCount = network.line_count;

    const lineCountElement = document.getElementById("line-count");

if (lineCountElement) {
    lineCountElement.textContent =
        `${String(activeCount).padStart(2, "0")} / 04`;
}

    document.querySelectorAll(".grid-line").forEach(line => {
        const id = Number(line.dataset.line);
        line.classList.toggle("failed", disabledLines.has(id));
    });

    document.querySelectorAll(".line-status").forEach(status => {
        const id = Number(status.dataset.status);
        const active = !disabledLines.has(id);

        status.textContent = active ? "● Active" : "● Failed";
        status.style.color = active ? "#43d6a0" : "#ff657a";
    });

    document.querySelectorAll(".line-toggle").forEach(button => {
        const id = Number(button.dataset.line);
        const isOff = disabledLines.has(id);

        button.textContent = isOff ? "Switch ON" : "Switch OFF";
        button.classList.toggle("is-off", isOff);
    });

    const info = document.getElementById("station-info");

    if (info) {
        info.textContent =
            `${network.substation_count} substations | ` +
            `${network.line_count} active lines | ` +
            `Generation: ${network.total_supply_mw} MW | ` +
            `Demand: ${network.total_demand_mw} MW`;
    }

    const networkStats = document.querySelectorAll(
        "#network .stats h2"
    );

    if (networkStats.length >= 2) {
        networkStats[0].textContent =
            String(network.substation_count).padStart(2, "0");

        networkStats[1].textContent =
            String(network.line_count).padStart(2, "0");
    }
}


// ==========================================
// SUBSTATION INSPECTION
// ==========================================

document.querySelectorAll(".station").forEach(station => {
    station.addEventListener("click", () => {
        const id = Number(station.dataset.station);

        document.querySelectorAll(".station").forEach(s => {
            s.classList.remove("selected");
        });

        station.classList.add("selected");

        const data = substations.find(s => s.id === id);

        document.getElementById("station-info").textContent =
            `${data.name} | Supply: ${data.supply_mw} MW | ` +
            `Demand: ${data.demand_mw} MW`;

        showToast(`${data.name} substation selected.`);
    });
});


// ==========================================
// LINE ON / OFF
// ==========================================

document.querySelectorAll(".line-toggle").forEach(button => {
    button.addEventListener("click", async () => {
        const id = Number(button.dataset.line);

        if (disabledLines.has(id)) {
            disabledLines.delete(id);
            showToast(`Transmission Line ${id} restored.`);
        } else {
            disabledLines.add(id);
            showToast(`Transmission Line ${id} switched OFF.`);
        }

        await analyzeGrid();
    });
});


// ==========================================
// SHORTEST PATH - C++ DIJKSTRA
// ==========================================

document.getElementById("calculate-path").addEventListener(
    "click",
    async () => {
        const source = Number(
            document.getElementById("path-source").value
        );

        const destination = Number(
            document.getElementById("path-destination").value
        );

        if (source === destination) {
            showToast("Choose two different substations.");
            return;
        }

        const result = await analyzeGrid();
        if (!result) return;

        renderShortestPath(result);

        showToast("C++ Dijkstra analysis completed!");
    }
);

function renderShortestPath(result) {
    const path = result.shortest_path;

    const routeElement = document.getElementById("path-route");
    const distanceElement = document.getElementById("path-distance");
    const messageElement = document.getElementById("path-message");

    if (!path.path_found) {
        routeElement.textContent = "No available route";
        distanceElement.innerHTML = "-- <span>km</span>";
        messageElement.textContent =
            "No route exists between the selected substations.";

        return;
    }

    const routeNames = path.path.map(id => stations[id]);

    routeElement.textContent = routeNames.join(" → ");

    distanceElement.innerHTML =
        `${Number(path.total_distance_km).toFixed(2)} <span>km</span>`;

    messageElement.textContent =
        `Calculated by C++ Dijkstra | ${path.weight_type} | ` +
        `${path.path.length} substations in route.`;
}


// ==========================================
// MAXIMUM FLOW - C++ EDMONDS-KARP
// ==========================================

document.getElementById("calculate-flow").addEventListener(
    "click",
    async () => {
        const result = await analyzeGrid();
        if (!result) return;

        renderMaximumFlow(result);

        showToast("C++ maximum-flow analysis completed!");
    }
);

function renderMaximumFlow(result) {
    const flow = result.maximum_flow;

    document.getElementById("flow-value").innerHTML =
        `${Number(flow.maximum_flow_mw).toFixed(2)} <span>MW</span>`;

    document.getElementById("flow-message").textContent =
        `All generators and loads considered by the C++ ` +
        `Edmonds–Karp implementation. Unmet demand: ` +
        `${Number(flow.unmet_demand_mw).toFixed(2)} MW.`;

    const cutContainer = document.getElementById("cut-lines");

    cutContainer.innerHTML = flow.minimum_cut_line_ids.length
        ? flow.minimum_cut_line_ids.map(id =>
            `<span class="chip">Line ${id}</span>`
        ).join("")
        : '<span class="chip">No cut lines</span>';
}


// ==========================================
// MST - C++ KRUSKAL AND PRIM
// ==========================================

document.getElementById("show-mst").addEventListener(
    "click",
    async () => {
        const result = await analyzeGrid();
        if (!result) return;

        const selected = result.mst.kruskal.selected_edges.map(
            edge => edge.line_id
        );

        document.querySelectorAll(".grid-line").forEach(line => {
            const id = Number(line.dataset.line);

            line.style.opacity =
                selected.includes(id) ? "1" : ".12";
        });

        document.getElementById("mst-message").textContent =
            `C++ Kruskal selected lines: ${selected.join(", ")}. ` +
            `Total cost: ${result.mst.kruskal.total_cost.toFixed(2)}.`;

        showToast("C++ MST connections highlighted.");
    }
);

function renderMST(result) {
    const kruskal = result.mst.kruskal;
    const prim = result.mst.prim;

    const mstCards = document.querySelectorAll("#mst .stats h2");

    if (mstCards.length >= 2) {
        mstCards[0].textContent = kruskal.total_cost.toFixed(2);
        mstCards[1].textContent = prim.total_cost.toFixed(2);
    }

    const tbody = document.querySelector("#mst .table-wrap tbody");

    if (tbody) {
        tbody.innerHTML = kruskal.selected_edges.map(edge => `
            <tr>
                <td>Line ${edge.line_id}</td>
                <td>${stations[edge.from]} — ${stations[edge.to]}</td>
                <td>${Number(edge.cost).toFixed(2)}</td>
            </tr>
        `).join("");
    }

    const message = document.getElementById("mst-message");

    if (!kruskal.is_spanning_tree) {
        message.textContent =
            `Network is disconnected. Kruskal produced a forest with ` +
            `${kruskal.component_count} components.`;
    }
}


// ==========================================
// FAULT TOLERANCE - C++ BRIDGE DETECTION
// ==========================================

document.getElementById("simulate-fault").addEventListener(
    "click",
    async () => {
        const id = Number(
            document.getElementById("fault-line").value
        );

        disabledLines.add(id);

        const result = await analyzeGrid();
        if (!result) return;

        const output = document.getElementById("fault-output");

        output.classList.add("failed-output");

        output.innerHTML = `
            <span class="table-status">● Line ${id} failed</span>
            <p>Connection ${stations[lines[id - 1].from]} →
            ${stations[lines[id - 1].to]} is unavailable.</p>
            <p>Active lines: ${result.network.line_count} / 4</p>
            <p>Remaining bridges: ${result.fault_tolerance.bridge_count}</p>
        `;

        showToast(`Fault simulated on Line ${id}.`);
    }
);


// ==========================================
// FAULT TOLERANCE RESULTS
// ==========================================

function renderFaultTolerance(result) {
    const fault = result.fault_tolerance;
    const augmentation = fault.augmentation;

    const cards = document.querySelectorAll("#fault .stats h2");

    if (cards.length >= 3) {
        cards[0].textContent = fault.bridges.length
            ? fault.bridges.map(id => `L${id}`).join(", ")
            : "None";

        cards[1].textContent = fault.bridge_count;
        cards[2].textContent = augmentation.final_bridge_count;
    }

    const augmentationGrid = document.querySelectorAll(
        "#fault .augmentation-grid h2"
    );

    if (augmentationGrid.length >= 3) {
        augmentationGrid[0].textContent =
            augmentation.selected_links.length
                ? `Line ${augmentation.selected_links.map(l => l.id).join(", ")}`
                : "None";

        augmentationGrid[1].textContent =
            Number(augmentation.budget_used).toFixed(2);

        augmentationGrid[2].textContent =
            Number(augmentation.budget_remaining).toFixed(2);
    }
}


// ==========================================
// RESTORE NETWORK
// ==========================================

document.getElementById("restore-grid").addEventListener(
    "click",
    async () => {
        disabledLines.clear();

        const result = await analyzeGrid();
        if (!result) return;

        const output = document.getElementById("fault-output");

        output.classList.remove("failed-output");

        output.innerHTML = `
            <span class="table-status">● Network stable</span>
            <p>All four transmission lines have been restored.</p>
            <p>Backend analysis completed successfully.</p>
        `;

        document.querySelectorAll(".grid-line").forEach(line => {
            line.style.opacity = "";
        });

        document.getElementById("mst-message").textContent =
            "Press the button to highlight the selected connections.";

        showToast("Network restored successfully!");
    }
);


// ==========================================
// DATA ENTRY - CUSTOM NETWORK MANAGEMENT
// ==========================================

function updateSubstationDropdowns() {
    const dropdownIds = [
        "path-source",
        "path-destination",
        "flow-source",
        "flow-destination"
    ];

    dropdownIds.forEach(id => {
        const dropdown = document.getElementById(id);
        if (!dropdown) return;

        const previousValue = dropdown.value;
        dropdown.innerHTML = "";

        substations.forEach(station => {
            const option = document.createElement("option");
            option.value = station.id;
            option.textContent =
                `${String(station.id).padStart(2, "0")} — ${station.name}`;
            dropdown.appendChild(option);
        });

        if (substations.some(s => String(s.id) === previousValue)) {
            dropdown.value = previousValue;
        }
    });
}

function renderEntrySummary() {
    const summary = document.getElementById("entry-summary");

    summary.innerHTML = `
        <p><strong>Substations:</strong> ${substations.length}</p>
        <p><strong>Transmission lines:</strong> ${lines.length}</p>
        <p><strong>Active lines:</strong> ${
            lines.filter(line => !disabledLines.has(line.id)).length
        }</p>
    `;
}

// ADD SUBSTATION
document.getElementById("add-station").addEventListener("click", () => {
    const id = Number(document.getElementById("station-id").value);
    const name = document.getElementById("station-name").value.trim();
    const type = document.getElementById("station-type").value;
    const supply = Number(document.getElementById("station-supply").value);
    const demand = Number(document.getElementById("station-demand").value);

    if (!Number.isInteger(id) || id <= 0 || !name) {
        showToast("Enter a valid ID and substation name.");
        return;
    }

    if (substations.some(s => s.id === id)) {
        showToast("This substation ID already exists.");
        return;
    }

    if (!Number.isFinite(supply) || !Number.isFinite(demand) ||
        supply < 0 || demand < 0) {
        showToast("Supply and demand must be non-negative numbers.");
        return;
    }

    if (type === "generator" && supply <= 0) {
        showToast("A generator must have positive supply.");
        return;
    }

    if (type === "load" && demand <= 0) {
        showToast("A load must have positive demand.");
        return;
    }

    const station = {
        id,
        name,
        x: substations.length,
        y: 0,
        type,
        supply_mw: supply,
        demand_mw: demand
    };

    substations.push(station);
    stations[id] = name;

    updateSubstationDropdowns();
    renderEntrySummary();

    document.getElementById("station-id").value = "";
    document.getElementById("station-name").value = "";
    document.getElementById("station-supply").value = "0";
    document.getElementById("station-demand").value = "0";

    document.getElementById("entry-message").textContent =
        `${name} substation added successfully.`;

    showToast("Substation added!");
});

// ADD TRANSMISSION LINE
document.getElementById("add-line").addEventListener("click", () => {
    const id = Number(document.getElementById("line-id").value);
    const from = Number(document.getElementById("line-from").value);
    const to = Number(document.getElementById("line-to").value);
    const distance = Number(document.getElementById("line-distance").value);
    const terrain = Number(document.getElementById("line-terrain").value);
    const capacity = Number(document.getElementById("line-capacity").value);

    if (!Number.isInteger(id) || id <= 0) {
        showToast("Enter a valid line ID.");
        return;
    }

    if (lines.some(line => line.id === id)) {
        showToast("This line ID already exists.");
        return;
    }

    if (from === to || !substations.some(s => s.id === from) ||
        !substations.some(s => s.id === to)) {
        showToast("Choose two different existing substations.");
        return;
    }

    if (![distance, terrain, capacity].every(Number.isFinite) ||
        distance <= 0 || terrain <= 0 || capacity <= 0) {
        showToast("Distance, terrain factor and capacity must be positive.");
        return;
    }

    lines.push({
        id,
        from,
        to,
        distance_km: distance,
        terrain_factor: terrain,
        capacity_mw: capacity
    });

    renderEntrySummary();

    document.getElementById("line-id").value = "";
    document.getElementById("line-from").value = "";
    document.getElementById("line-to").value = "";
    document.getElementById("line-distance").value = "";
    document.getElementById("line-capacity").value = "";

    document.getElementById("entry-message").textContent =
        `Transmission line ${id} added successfully.`;

    showToast("Transmission line added!");
});

// ANALYZE CUSTOM NETWORK
document.getElementById("analyze-custom-grid").addEventListener(
    "click",
    async () => {
        if (substations.length < 2 || lines.length < 1) {
            showToast("Add at least two substations and one line.");
            return;
        }

        const message = document.getElementById("entry-message");
        message.textContent = "Sending custom network to C++ backend...";

        const result = await analyzeGrid();

        if (!result) {
            message.textContent = "Analysis failed. Check backend connection.";
            return;
        }

        renderEntrySummary();

        message.textContent =
            `Analysis successful! ${result.network.substation_count} substations ` +
            `and ${result.network.line_count} active lines analyzed by C++.`;

        showToast("Custom network analysis completed!");
    }
);

renderEntrySummary();
updateSubstationDropdowns();

// ==========================================
// INITIALIZATION
// ==========================================

analyzeGrid();

console.log("GridGuard frontend connected to C++ API.");
